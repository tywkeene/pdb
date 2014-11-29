#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdb.h"

void pdb_free_table_entry(table_entry_t *e)
{
    if(e == NULL)
        return;
    free(e->entry_name);
    free(e->entry_data);
    free(e);
    return;
}

void pdb_free_table(table_t *t)
{
    unsigned int i;
    for(i = 0; i < t->table_entry_count; i++)
        pdb_free_table_entry(t->table_entries[i]);
    free(t->table_entries);
    free(t);
    return;
}

void pdb_free_table_set(table_set_t *ts)
{
    unsigned int i;
    for(i = 0; i < ts->max_tables; i++)
        pdb_free_table(ts->table_set[i]);
    free(ts->table_set);
    free(ts);
    return;
}

table_entry_t *pdb_alloc_table_entry(unsigned int id, unsigned int parent_id,
        const char *name, const char *entry)
{
    table_entry_t *e = malloc(sizeof(table_entry_t));

    e->entry_data = malloc(strlen(entry) + 1);
    strncpy(e->entry_data, entry, strlen(entry) + 1);

    e->entry_name = malloc(strlen(name) + 1);
    strncpy(e->entry_name, name, strlen(name) + 1);

    e->entry_id = id;
    e->parent_table_id = parent_id;
    e->entry_timestamp = time(NULL);

    return e;
}

table_t *pdb_alloc_table(unsigned int id, unsigned int parent_id, unsigned int max_entries)
{
    table_t *t = malloc(sizeof(table_t));
    t->table_entries = calloc(max_entries, sizeof(table_entry_t *));
    t->table_id = id;
    t->parent_set_id = parent_id;
    t->table_entry_count = 0;
    t->table_timestamp = time(NULL);
    return t;
}

table_set_t *pdb_alloc_table_set(unsigned int id, unsigned int max_tables)
{
    table_set_t *ts = malloc(sizeof(table_set_t));
    ts->table_set = calloc(max_tables, sizeof(table_t *));
    ts->set_id = id;
    ts->set_table_count = 0;
    ts->max_tables = max_tables;
    ts->set_timestamp = time(NULL);
    return ts;
}
