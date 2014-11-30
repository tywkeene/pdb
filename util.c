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
    if(pdb_errno > sizeof_array(err_strs) || pdb_errno < 1)
        return err_strs[0];
    return err_strs[pdb_errno];
}

void pdb_add_table_entry(table_t *t, table_entry_t *e)
{
    t->table_entries[t->table_entry_count++] = e;
}

void pdb_add_table(table_set_t *ts, table_t *t)
{
    if((ts->set_table_count + 1) > ts->max_tables){
        pdb_errno = ERROR_TS_OUT_OF_RANGE;
        return;
    }
    ts->table_set[ts->set_table_count++] = t;
}

table_t *pdb_get_table(table_set_t *ts, unsigned int table)
{
    if(table > ts->set_table_count)
        return NULL;
    return ts->table_set[table];
}

table_entry_t *pdb_get_entry(table_t *t, unsigned int entry)
{
    if(entry > t->table_entry_count)
        return NULL;
    return t->table_entries[entry];
}

unsigned int pdb_get_table_count(table_set_t *ts)
{
    return ts->set_table_count;
}

unsigned int pdb_get_entry_count(table_t *t)
{
    return t->table_entry_count;
}
