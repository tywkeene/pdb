#ifndef PDB_H
#define PDB_H

typedef struct field_t{
    char *field_name;
    char *field_entry;
    unsigned int type;
    unsigned int field_id;
    unsigned int parent_table;
    struct field_t *next;
}field_t;

typedef struct field_format_t{
    char **field_format;
    unsigned int max_fields;
}field_format_t;

typedef struct table_t{
    field_t *fields;
    field_format_t *format;
    unsigned int table_id;
    unsigned int field_count;
    struct table_t *next;
}table_t;

#endif
