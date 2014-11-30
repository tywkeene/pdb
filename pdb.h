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

/*Error magic numbers, used in pdb_get_error() to get a descriptive string
 * of the most recent error*/
#define ERROR_BAD_VERSION 1
#define ERROR_INVALID_FILE 2
#define ERROR_IO 3

#define sizeof_array(x) (sizeof(x) / sizeof(x[0]))

/*Global variables of various uses*/

/*Holds the error number of the most recent error (defined above)*/
int pdb_errno;

/*File header - Holds length of table entry
 * name/data and the timestamp of the entry*/
typedef struct table_entry_header_t{
    unsigned int name_len;
    unsigned int data_len;
    time_t timestamp;
}__attribute__((packed)) table_entry_header_t;

/*File header - Holds the count of entries
 * in the table and the timstamp for that table*/
typedef struct table_header_t{
    unsigned int entries;
    time_t timestamp;
}__attribute__((packed)) table_header_t;

/*File header - Holds the PDB version of the file,
 * number of tables and timestamp of the set*/
typedef struct file_header_t{
    int pdb_version_major;
    unsigned int tables;
    time_t timestamp;
}__attribute__((packed)) file_header_t;

/*Table entry (or 'collumn' in sql-speak). Holds the actual
 * data name/data, an id, the id of the parent table and the entry timestamp*/
typedef struct table_entry_t{
    char *entry_data;
    char *entry_name;
    unsigned int entry_id;
    unsigned int parent_table_id;
    time_t entry_timestamp;
}table_entry_t;

/*Table (or row in sql-speak). Holds a variable number of table entries
 * in table_entries, a table id, a parent set id, number of tables and the timestamp*/
typedef struct table_t{
    table_entry_t **table_entries;
    unsigned int table_id;
    unsigned int parent_set_id;
    unsigned int table_entry_count;
    time_t table_timestamp;
}table_t;

/*Table set (or file in sql-speak, I guess?) Holds a variable amount of tables,
 * the set id, max tables, current tables in this set and the timestamp*/
typedef struct table_set_t{
    table_t **table_set;
    unsigned int set_id;
    unsigned int max_tables;
    unsigned int set_table_count;
    time_t set_timestamp;
}table_set_t;

/*
All functions are in order of appearance in their respective files
*/

/*util.c*/
/*Get version string, major version, minor version and release version*/
char *pdb_get_version_string(void);
int pdb_get_version_major(void);
int pdb_get_version_minor(void);
int pdb_get_version_release(void);

/*Get error string*/
const char *pdb_get_error(void);

/*Helper functions to add tables/entries to table sets/tables, respectively*/
void pdb_add_table_entry(table_t *, table_entry_t *);
void pdb_add_table(table_set_t *, table_t *);

/*Helper functions to get table/entry pointers*/
table_t *pdb_get_table(table_set_t *, unsigned int);
table_entry_t *pdb_get_entry(table_t *, unsigned int);

/*Helper functions to get table count in a set and entry count in a table*/
unsigned int pdb_get_table_count(table_set_t *);
unsigned int pdb_get_entry_count(table_t *t);

/*pdb.c*/
/*Free table entries, tables and table sets*/
void pdb_free_table_entry(table_entry_t *);
void pdb_free_table(table_t *);
void pdb_free_table_set(table_set_t *);

/*Allocate new table entries, tables and table sets*/
table_entry_t *pdb_alloc_table_entry(unsigned int, unsigned int,
        const char *, const char *);
table_t *pdb_alloc_table(unsigned int, unsigned int, unsigned int);
table_set_t *pdb_alloc_table_set(unsigned int, unsigned int);

/*io.c*/
/*Write table entry headers, table headers, file headers and table sets*/
__attribute__((unused)) static int pdb_write_table_entry(FILE *, table_entry_t *);
__attribute__((unused)) static int pdb_write_table_header(FILE *, table_t *);
__attribute__((unused)) static int pdb_write_file_header(FILE *, table_set_t *);
__attribute__((unused)) int pdb_write_table_set(table_set_t *, const char *);

/*Read table entries, tables and table sets*/
__attribute__((unused)) static table_entry_t *pdb_read_table_entry(FILE *, unsigned int, unsigned int);
__attribute__((unused)) static table_t *pdb_read_table_header(FILE *, unsigned int, unsigned int);
table_set_t *pdb_read_table_set(const char *, unsigned int);

#endif
