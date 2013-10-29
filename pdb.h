#ifndef PDB_H
#define PDB_H

typedef struct table_entry_t{
    char *entry_name;
    char *entry_data;
    unsigned int type;
    unsigned int table_entry_id;
    unsigned int parent_table;
}table_entry_t;

typedef struct entry_format_t{
    char **entry_format;
    unsigned int max_entries;
}entry_format_t;

typedef struct table_t{
    table_entry_t **entries;
    entry_format_t *format;
    unsigned int table_id;
    unsigned int entry_count;
}table_t;

#endif
