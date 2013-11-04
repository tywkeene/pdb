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

#endif
