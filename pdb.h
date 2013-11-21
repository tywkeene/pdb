#ifndef PDB_H
#define PDB_H

typedef struct table_entry_t{
    char *entry_data;
    char *entry_name;
    unsigned int entry_id;
    unsigned int parent_table_id;
    unsigned int entry_timestamp;
}table_entry_t;

typedef struct table_t{
    table_entry_t **table_entries;
    unsigned int table_id;
    unsigned int parent_set_id;
    unsigned int table_timestamp;
    unsigned int table_entry_count;
}table_t;

typedef struct table_entry_format_t{
    char **format;
    unsigned int max_entries;
}table_entry_format_t;

typedef struct table_set_t{
    table_t **table_set;
    table_entry_format_t *entry_format;
    unsigned int set_id;
    unsigned int max_tables;
    unsigned int set_table_count;
    unsigned int set_timestamp;
}table_set_t;

/*All functions are in order of appearance in their respective files*/

/*compression.c*/
int def(const char *, const char *, int);
int inf(const char *, const char *);
int zerr(int);

/*pdb.c*/
void free_table_entry(table_entry_t *);
void free_table(table_t *);
void free_table_format(table_entry_format_t *);
void free_table_set(table_set_t *);
table_entry_t *alloc_table_entry(unsigned int, const char *, unsigned int, 
        table_t *, table_entry_format_t *);
table_t *alloc_table(unsigned int, unsigned int, unsigned int);
table_entry_format_t *alloc_entry_format(unsigned int);
table_set_t *alloc_table_set(unsigned int, unsigned int);
unsigned int count_token(char *, const char *);
table_set_t *read_file_format(char *, unsigned int);
table_set_t *parse_input_file(const char *, unsigned int);
int decompress_database(const char *, const char *);
int compress_database(const char *, const char *);

#endif
