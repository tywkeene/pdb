#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdb.h"

char *pdb_get_version_string(void)
{
    return PDB_VERSION_STRING;
}

int pdb_get_version_major(void)
{
    return PDB_VERSION_MAJOR;
}

int pdb_get_version_minor(void)
{
    return PDB_VERSION_MINOR;
}

int pdb_get_version_release(void)
{
    return PDB_VERSION_RELEASE;
}

const char *pdb_get_error(void)
{
    const char *err_strs[] = {
        "No error", 
        "Bad version in file header",
        "Invalid file format", 
        "Input/Output error"
    };
    return err_strs[pdb_errno];
}

void pdb_free_table_entry(table_entry_t *e)
{
    if(e == NULL)
        return;
    free(e->entry_data);
    free(e);
    return;
}

void pdb_free_table(table_t *t)
{
    unsigned int i;
    for(i = 0; i < t->table_entry_count; i++)
        pdb_free_table_entry(t->table_entries[i]);
    free(t->table_entries);
    free(t);
    return;
}

void pdb_free_table_set(table_set_t *ts)
{
    unsigned int i;
    for(i = 0; i < ts->max_tables; i++)
        pdb_free_table(ts->table_set[i]);
    free(ts->table_set);
    free(ts);
    return;
}

table_entry_t *pdb_alloc_table_entry(unsigned int id, unsigned int parent_id, 
        const char *name, const char *entry)
{
    table_entry_t *e;
    e = malloc(sizeof(table_entry_t));

    e->entry_data = malloc(strlen(entry) + 1);
    strncpy(e->entry_data, entry, strlen(entry) + 1);

    e->entry_name = malloc(strlen(name) + 1);
    strncpy(e->entry_name, name, strlen(name) + 1);

    e->entry_id = id;
    e->parent_table_id = parent_id;
    e->entry_timestamp = time(NULL);

    return e;
}

table_t *pdb_alloc_table(unsigned int id, unsigned int parent_id, unsigned int max_entries)
{
    table_t *t;
    t = malloc(sizeof(table_t));
    t->table_entries = calloc(max_entries, sizeof(table_entry_t *));
    t->table_id = id;
    t->parent_set_id = parent_id;
    t->table_entry_count = 0;
    t->table_timestamp = time(NULL);
    return t;
}

table_set_t *pdb_alloc_table_set(unsigned int id, unsigned int max_tables)
{
    table_set_t *ts;
    ts = malloc(sizeof(table_set_t));
    ts->table_set = calloc(max_tables, sizeof(table_t *));
    ts->set_id = id;
    ts->set_table_count = 0;
    ts->max_tables = max_tables;
    ts->set_timestamp = time(NULL);
    return ts;
}

static int pdb_write_table_entry(FILE *fd, table_entry_t *e)
{
    struct table_entry_header_t header = {.name_len = strlen(e->entry_name), 
    .data_len = strlen(e->entry_data), .timestamp = e->entry_timestamp};

    if(fwrite(&header, PDB_ENTRY_HEADER_LEN, 1, fd) != 1){
        pdb_errno = ERROR_IO;
        return -1;
    }
    fprintf(fd, "%s", e->entry_name);
    fprintf(fd, "%s", e->entry_data);
    fflush(fd);
    return 0;
}

static int pdb_write_table_header(FILE *fd, table_t *t)
{
    struct table_header_t header = {.entries = t->table_entry_count, 
        .timestamp = t->table_timestamp};
    if(fwrite(&header, PDB_TABLE_HEADER_LEN, 1, fd) != 1){
        pdb_errno = ERROR_IO;
        return -1;
    }
    fflush(fd);
    return 0;
}

static int pdb_write_file_header(FILE *fd, table_set_t *ts)
{
    struct file_header_t header = {
        .pdb_version_major = PDB_VERSION_MAJOR,
        .tables = ts->set_table_count, 
        .timestamp = ts->set_timestamp};
    if(fwrite(&header, PDB_FILE_HEADER_LEN, 1, fd) != 1){
        pdb_errno = ERROR_IO;
        return -1;
    }
    fflush(fd);
    return 0;
}

int pdb_write_table_set(table_set_t *ts, const char *filename)
{
    FILE *fd;
    table_t *t;
    table_entry_t *e;
    unsigned int cur_table;
    unsigned int cur_entry;

    if((fd = fopen(filename, "w+b")) == NULL){
        pdb_errno = ERROR_IO;
        return -1;
    }

    if(pdb_write_file_header(fd, ts) == -1){
        pdb_errno = ERROR_IO;
        return -1;
    }

    for(cur_table = 0; cur_table < ts->set_table_count; cur_table++){
        t = ts->table_set[cur_table];
        pdb_write_table_header(fd, t);
        for(cur_entry = 0; cur_entry < t->table_entry_count; cur_entry++){
            e = t->table_entries[cur_entry];
            pdb_write_table_entry(fd, e);
        }
    }
    fclose(fd);
    return 0;
}

table_entry_t *pdb_read_table_entry(FILE *fd, unsigned int id, unsigned int parent_id)
{
    struct table_entry_header_t header;
    table_entry_t *e = malloc(sizeof(table_entry_t));

    fread(&header, PDB_ENTRY_HEADER_LEN, 1, fd);
    e->entry_name = malloc(header.name_len + 1);
    e->entry_data = malloc(header.data_len + 1);
    fgets(e->entry_name, header.name_len + 1, fd);
    fgets(e->entry_data, header.data_len + 1, fd);

    e->parent_table_id = parent_id;
    e->entry_id = id;
    e->entry_timestamp = header.timestamp;

    return e;
}

table_t *pdb_read_table_header(FILE *fd, unsigned int table_id, unsigned int parent_id)
{
    table_t *t = malloc(sizeof(table_t));
    unsigned int cur_entry;
    struct table_header_t header;

    fread(&header, PDB_TABLE_HEADER_LEN, 1, fd);
    t->table_id = table_id;
    t->parent_set_id = parent_id;
    t->table_timestamp = header.timestamp;
    t->table_entry_count = header.entries;
    t->table_entries = malloc(sizeof(table_entry_t *) * header.entries);

    for(cur_entry = 0; cur_entry < t->table_entry_count; cur_entry++)
        t->table_entries[cur_entry] = pdb_read_table_entry(fd, table_id, table_id);
    return t;
}

table_set_t *pdb_read_table_set(const char *filename, unsigned int id)
{
    FILE *fd;
    unsigned int cur_table;
    table_set_t *ts = malloc(sizeof(table_set_t));
    struct file_header_t file_header;

    if((fd = fopen(filename, "r+b")) == NULL){
        pdb_errno = ERROR_IO;
        return NULL;
    }

    fread(&file_header, PDB_FILE_HEADER_LEN, 1, fd);

    if(file_header.pdb_version_major != PDB_VERSION_MAJOR){
        pdb_errno = ERROR_BAD_VERSION;
        return NULL;
    }

    ts->set_id = id;
    ts->set_table_count = 0;
    ts->max_tables = file_header.tables;
    ts->table_set = malloc(sizeof(table_t *) * file_header.tables);
    ts->set_timestamp = file_header.timestamp;

    for(cur_table = 0; cur_table < file_header.tables; cur_table++)
        ts->table_set[cur_table] = pdb_read_table_header(fd, cur_table, ts->set_id);
    return ts;
}
