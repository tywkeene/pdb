#ifndef PDB_H
#define PDB_H

#include <time.h>

#define PDB_VERSION_MAJOR 1
#define PDB_VERSION_MINOR 0
#define PDB_VERSION_RELEASE 0
#define PDB_VERSION_STRING "libpdb (Version 1.0.0)"

#define PDB_FILE_HEADER_LEN (sizeof(struct file_header_t))
#define PDB_TABLE_HEADER_LEN (sizeof(struct table_header_t))
#define PDB_ENTRY_HEADER_LEN (sizeof(struct table_entry_header_t))

#define ERROR_BAD_VERSION 1
#define ERROR_INVALID_FILE 2
#define ERROR_IO 3

/*Holds the error number of the most recent error (defined above*/
int pdb_errno;

typedef struct table_entry_header_t{
    unsigned int name_len;
    unsigned int data_len;
    time_t timestamp;
}__attribute__((packed)) table_entry_header_t;

typedef struct table_header_t{
    unsigned int entries;
    time_t timestamp;
}__attribute__((packed)) table_header_t;

typedef struct file_header_t{
    int pdb_version_major;
    unsigned int tables;
    time_t timestamp;
}__attribute__((packed)) file_header_t;

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

/*
All functions are in order of appearance in their respective files
These functions are in pdb.c
*/

/*Get version string, major version, minor version and release version*/
char *pdb_get_version_string(void);
int pdb_get_version_major(void);
int pdb_get_version_minor(void);
int pdb_get_version_release(void);

/*Get error string*/
const char *pdb_get_error(void);

/*Free table entries, tables and table sets*/
void pdb_free_table_entry(table_entry_t *);
void pdb_free_table(table_t *);
void pdb_free_table_set(table_set_t *);

/*Allocate new table entries, tables and table sets*/
table_entry_t *pdb_alloc_table_entry(unsigned int, unsigned int, 
        const char *, const char *);
table_t *pdb_alloc_table(unsigned int, unsigned int, unsigned int);
table_set_t *pdb_alloc_table_set(unsigned int, unsigned int);

/*Write table entry headers, table headers, file headers and table sets*/
static int pdb_write_table_entry(FILE *, table_entry_t *);
static int pdb_write_table_header(FILE *, table_t *);
static int pdb_write_file_header(FILE *, table_set_t *);
int pdb_write_table_set(table_set_t *, const char *);

/*Read table entries, tables and table sets*/
table_entry_t *pdb_read_table_entry(FILE *, unsigned int, unsigned int);
table_t *pdb_read_table_header(FILE *, unsigned int, unsigned int);
table_set_t *pdb_read_table_set(const char *, unsigned int);

#endif
