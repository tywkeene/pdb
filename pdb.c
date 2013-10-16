#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/mman.h> 
#include <sys/types.h>

#include "pdb.h"

field_t *alloc_field(const char *field_entry, field_type_t type, table_t *t)
{
    field_t *pntr;
    if(field_entry == NULL){
        fprintf(stderr, "Refusing to add NULL entry\n");
        return NULL;
    }
    pntr = malloc(sizeof(field_t));
    pntr->field_entry = malloc(strlen(field_entry) + 1);
    strncpy(pntr->field_entry, field_entry, strlen(field_entry) + 1);
    pntr->field_name = (char *) field_names[type];
    pntr->type = type;
    pntr->next = NULL;
    pntr->field_id = t->field_count++;
    pntr->parent_table = t->table_id;
    fprintf(stdout, "[Table %u]: Allocated field %u with name:entry '%s: %s'\n",
            t->table_id, pntr->field_id, pntr->field_name, pntr->field_entry);
    return pntr;
}

int free_field(field_t *pntr)
{
    if(pntr->field_entry != NULL)
        free(pntr->field_entry);
    free(pntr);
    return 1;
}

void free_table(table_t *t)
{
    field_t **curr;
    field_t *field;
    fprintf(stdout, "Freeing table %u\n", t->table_id);
    if(t->fields != NULL)
        for(curr = &t->fields; *curr != NULL;){
            field = *curr;
            *curr = field->next;
            fprintf(stdout, "[Table %u]: Freeing field entry %u\n", t->table_id,
                    field->field_id);
            free_field(field);
        }
    free(t);
    return;
}

table_t *alloc_table(unsigned short id)
{
    table_t *pntr;
    pntr = malloc(sizeof(table_t));
    pntr->table_id = id; 
    pntr->field_count = 0;
    pntr->fields = NULL;
    pntr->next = NULL;
    fprintf(stdout, "Allocated table %u\n", id);
    return pntr;
}

int parse_input_file(const char *file_path, table_t *t)
{
    int input_fd;
    char *buffer;
    char *token;
    char *string;
    char *data_pntr;
    struct stat st;
    off_t file_size = 0;

    if((input_fd = open(file_path, O_RDONLY)) == -1){
        perror("open");
        return -1;
    }
    if(fstat(input_fd, &st) < 0){
        perror("stat");
        return -1;
    }
    file_size = st.st_size;
    fprintf(stdout, "File size: %jd\n", file_size);
    if((data_pntr = mmap(NULL, file_size, PROT_READ, 
                    MAP_PRIVATE | MAP_POPULATE, input_fd, 0)) == MAP_FAILED){
        perror("mmap");
        return -1;
    }
    if(data_pntr == NULL){
        perror("mmap");
        return -1;
    }

    buffer = malloc(file_size);
    memcpy(buffer, data_pntr, file_size);
    munmap(data_pntr, file_size);

    /*TODO:get string, get tokens, put tokens into fields*/

    free(buffer);
    return 0;
}

int main(int argc, char **argv)
{
    table_t *table;
    if(parse_input_file(argv[1], table) == -1)
        return -1;
    return 0;
}
