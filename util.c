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
    if(pdb_errno > sizeof(err_strs))
        return err_strs[0];
    return err_strs[pdb_errno];
}

void pdb_add_table_entry(table_t *t, table_entry_t *e)
{
    t->table_entries[t->table_entry_count++] = e;
}

void pdb_add_table(table_set_t *ts, table_t *t)
{
    ts->table_set[ts->set_table_count++] = t;
}

unsigned int pdb_get_table_count(table_set_t *ts)
{
    return ts->set_table_count;
}

unsigned int pdb_get_entry_count(table_t *t)
{
    return t->table_entry_count;
}
