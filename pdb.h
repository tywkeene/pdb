#ifndef PDB_H
#define PDB_H

#include <time.h>

typedef struct table_entry_t{
    char *entry_data;
    char *entry_name;
    unsigned int entry_id;
    unsigned int parent_table_id;
    time_t entry_timestamp;
}table_entry_t;

typedef struct table_t{
    table_entry_t **table_entries;
    unsigned int table_id;
    unsigned int parent_set_id;
    unsigned int table_entry_count;
    time_t table_timestamp;
}table_t;

typedef struct table_set_t{
    table_t **table_set;
    unsigned int set_id;
    unsigned int max_tables;
    unsigned int set_table_count;
    time_t set_timestamp;
}table_set_t;

/*All functions are in order of appearance in their respective files*/

/*pdb.c*/
void free_table_entry(table_entry_t *);
void free_table(table_t *);
void free_table_set(table_set_t *);
table_entry_t *alloc_table_entry(unsigned int, unsigned int, const char *, const char *);
table_t *alloc_table(unsigned int, unsigned int, unsigned int);
table_set_t *alloc_table_set(unsigned int, unsigned int);
unsigned int count_token(char *, const char *);
table_set_t *read_file_format(char *, unsigned int);
table_set_t *parse_input_file(const char *, unsigned int);
void print_table_entry(table_entry_t *);
int slow_search(table_set_t *, const char *);
int decompress_database(const char *, const char *);
int compress_database(const char *, const char *);

#endif
