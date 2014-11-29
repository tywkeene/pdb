#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdb.h"

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

static table_entry_t *pdb_read_table_entry(FILE *fd, unsigned int id, unsigned int parent_id)
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

static table_t *pdb_read_table_header(FILE *fd, unsigned int table_id, unsigned int parent_id)
{
    table_t *t = malloc(sizeof(table_t));
    unsigned int cur_entry;
    struct table_header_t header;

    fread(&header, PDB_TABLE_HEADER_LEN, 1, fd);

    fprintf(stdout, "<table header> ID: %u\tParent: %u\tTimestamp: %ld\tEntries: %u\n",
            table_id, parent_id, header.timestamp, header.entries);

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

    fprintf(stdout, "<file header> File: %s\t ID: %u\tFile version major: %d\tTimestamp: %ld\tTables: %u\n",
            filename, id, file_header.pdb_version_major,file_header.timestamp, file_header.tables);

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
    fclose(fd);
    return ts;
}
