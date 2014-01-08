#include <stdio.h>

#include "pdb.h"

int main(void)
{
    table_set_t *ts = pdb_alloc_table_set(0, 1);
    ts->table_set[0] = pdb_alloc_table(0, 0, 1);
    ts->set_table_count++;
    ts->table_set[0]->table_entries[0] = pdb_alloc_table_entry(0, 0, "Name", "Entry");
    ts->table_set[0]->table_entry_count++;

    table_entry_t *e = ts->table_set[0]->table_entries[0];

    fprintf(stdout, "Writing name '%s' and data '%s'\n",
            e->entry_name, e->entry_data);

    if(pdb_write_table_set(ts, "file") != 0)
        fprintf(stderr, "%s\n", pdb_get_error());

    pdb_free_table_set(ts);

    if((ts = pdb_read_table_set("file", 0)) == NULL)
        fprintf(stderr, "%s\n", pdb_get_error());

    e = ts->table_set[0]->table_entries[0];
    fprintf(stdout, "Read name '%s' and data '%s'\n",
            e->entry_name, e->entry_data);
    return 0;
}
