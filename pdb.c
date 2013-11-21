#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <sys/types.h>

#include "pdb.h"

void free_table_entry(table_entry_t *e)
{
    if(e == NULL)
        return;
    free(e->entry_data);
    free(e);
    return;
}

void free_table(table_t *t)
{
    unsigned int i;
    for(i = 0; i < t->table_entry_count; i++)
        free_table_entry(t->table_entries[i]);
    free(t->table_entries);
    free(t);
    return;
}

void free_table_format(table_entry_format_t *f)
{
    unsigned int i;
    if(f == NULL)
        return;
    for(i = 0; i < f->max_entries; i++)
        free(f->format[i]);
    free(f->format);
    free(f);
    return;
}

void free_table_set(table_set_t *ts)
{
    unsigned int i;
    fprintf(stdout, "Freeing table set [%u] with %u/%u tables [%u entries per table]\n",
            ts->set_id, ts->set_table_count, ts->max_tables, ts->entry_format->max_entries);
    for(i = 0; i < ts->max_tables; i++)
        free_table(ts->table_set[i]);
    free(ts->table_set);
    free_table_format(ts->entry_format);
    free(ts);
    return;
}

table_entry_t *alloc_table_entry(unsigned int id, const char *entry, unsigned int type, 
        table_t *t, table_entry_format_t *f)
{
    table_entry_t *e;
    e = malloc(sizeof(table_entry_t));
    e->entry_data = malloc(strlen(entry) + 1);
    strncpy(e->entry_data, entry, strlen(entry) + 1);

    e->entry_name = f->format[type];

    e->entry_id = id;
    e->parent_table_id = t->table_id;
    e->entry_timestamp = time(NULL);

    fprintf(stdout, "Entry Name: %s\nEntry Data: %s\nEntry ID: %u\nParent table ID: %u\nTimestamp: %u\n\n",
            e->entry_name, e->entry_data, e->entry_id, e->parent_table_id, e->entry_timestamp);

    return e;
}

table_t *alloc_table(unsigned int id, unsigned int parent_id, unsigned int max_entries)
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

table_entry_format_t *alloc_entry_format(unsigned int max_entries)
{
    table_entry_format_t *f;
    f = malloc(sizeof(table_entry_format_t));
    f->format = calloc(max_entries, sizeof(char *));
    f->max_entries = max_entries;
    return f;
}

table_set_t *alloc_table_set(unsigned int id, unsigned int max_tables)
{
    table_set_t *ts;
    ts = malloc(sizeof(table_set_t));
    ts->table_set = calloc(max_tables, sizeof(table_t *));
    ts->entry_format = NULL;
    ts->set_id = id;
    ts->set_table_count = 0;
    ts->max_tables = max_tables;
    ts->set_timestamp = time(NULL);
    return ts;
}

unsigned int count_token(char *string, const char *token_string)
{
    unsigned int count = 0;
    char *token;
    char *copy;
    copy = malloc(strlen(string) + 1);
    strncpy(copy, string, strlen(string) + 1);
    for(token = strtok(copy, token_string); token != NULL; 
            token = strtok(NULL, token_string))
        count++;
    free(copy);
    return count;
}

table_set_t *read_file_format(char *format_string, unsigned int set_count)
{
    char *token;
    char *copy;
    char *format_string_copy;
    unsigned int i;
    unsigned int num_entries;
    unsigned int num_tables;
    table_set_t *ts;

    copy = malloc(strlen(format_string) + 1);
    strncpy(copy, format_string, strlen(format_string) + 1);

    /*Count how many lines the table_set will hold*/
    num_tables = count_token(copy, "\r\n");

    /*Count how many entries are in a line, and that a table will hold*/
    format_string_copy = strtok(copy, "\r\n");
    num_entries = count_token(format_string_copy, "|");

    ts = alloc_table_set(set_count, num_tables);
    ts->entry_format = alloc_entry_format(num_entries);

    fprintf(stdout, "Number of tables in a set: %u\n", ts->max_tables);
    fprintf(stdout, "Number of entries per table: %u\n", ts->entry_format->max_entries);
    token = strtok(format_string_copy, "|");
    for(i = 0; i < num_entries; i++){
        ts->entry_format->format[i] = malloc(strlen(token) + 1);
        strncpy(ts->entry_format->format[i], token, strlen(token) + 1);
        if((token = strtok(NULL, "|")) == NULL)
            break;
    }
    fprintf(stdout, "Got field format\n");
    free(copy);
    return ts;
}

table_set_t *parse_input_file(const char *file_path, unsigned int set_count)
{
    int input_fd;
    char *copy;
    char *token;
    char *data_pntr;
    struct stat st;
    off_t file_size = 0;
    unsigned int entry = 0;
    unsigned int line = 0;
    table_set_t *ts;

    fprintf(stdout, "Opening file: %s\n", file_path);

    if((input_fd = open(file_path, O_RDONLY)) == -1){
        perror("open");
        return NULL;
    }
    if(fstat(input_fd, &st) < 0){
        close(input_fd);
        perror("stat");
        return NULL;
    }
    file_size = st.st_size;
    fprintf(stdout, "File size: %jd bytes\n", file_size);
    if((data_pntr = mmap(NULL, file_size, PROT_READ, 
                    MAP_PRIVATE | MAP_POPULATE, input_fd, 0)) == MAP_FAILED){
        close(input_fd);
        perror("mmap");
        return NULL;
    }
    if(data_pntr == NULL){
        close(input_fd);
        perror("mmap");
        return NULL;
    }

    close(input_fd);
    copy = malloc(file_size + 1);
    strncpy(copy, data_pntr, file_size + 1);
    munmap(data_pntr, file_size);

    ts = read_file_format(copy, set_count);
    token = strtok(copy, "|");
    ts->table_set[line] = alloc_table(ts->set_table_count++, ts->set_id, 
            ts->entry_format->max_entries);

    while(token != NULL){
        ts->table_set[line]->table_entries[entry] = alloc_table_entry(ts->table_set[line]->table_entry_count++, 
                token, entry, ts->table_set[line], ts->entry_format);
        token = strtok(NULL, "|");
        if(entry == ts->entry_format->max_entries - 1){
            if((token = strtok(NULL, "|")) == NULL)
                break;
            line++;
            entry = 0;
            ts->table_set[line] = alloc_table(ts->set_table_count++, ts->set_id, 
                    ts->entry_format->max_entries);
            fprintf(stdout, "Table %u\n-----\n", ts->table_set[line]->table_id);
        }
        else
            entry++;
    }
    free(copy);
    return ts;
}

int decompress_database(const char *filename_in, const char *filename_out)
{
    return zerr(inf(filename_in, filename_out));
}

int compress_database(const char *filename_in, const char *filename_out)
{
    return zerr(def(filename_in, filename_out, 9));
}

int main(int argc, char **argv)
{
    table_set_t *ts = NULL;

    if(argv[1] == NULL){
        fprintf(stdout, "usage: %s <filename>\n", argv[0]);
        return -1;
    }
    if(decompress_database(argv[1], "database.swap.pdb") != 0)
        return -1;

    ts = parse_input_file("database.swap.pdb", 0);

    if(compress_database("database.swap.pdb", argv[1]))
        return -1;
    unlink("database.swap.pdb");
    return 0;
}
