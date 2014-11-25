#include <stdio.h>

#include "pdb.h"

int main(void)
{
    fprintf(stdout, "%s\n", pdb_get_version_string());
    table_set_t *ts = pdb_alloc_table_set(0, 1, "Table set");

    pdb_add_table(ts, pdb_alloc_table(0, 0, 1, "Table"));
    pdb_add_table_entry(ts->table_set[0], pdb_alloc_table_entry(0, 0, "Name", "Entry"));

    table_entry_t *e = ts->table_set[0]->table_entries[0];

    fprintf(stdout, "Writing name '%s' and data '%s'\n",
            e->entry_name, e->entry_data);

    if(pdb_write_table_set(ts, "file_test") != 0)
        fprintf(stderr, "%s\n", pdb_get_error());

    pdb_free_table_set(ts)

    if((ts = pdb_read_table_set("file_test", 0)) == NULL)
        fprintf(stderr, "%s\n", pdb_get_error());

    fprintf(stdout, "Read name '%s' and data '%s'\n",
            e->entry_name, e->entry_data);

    pdb_free_table_set(ts);
    return 0;
}
