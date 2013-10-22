#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/mman.h> 
#include <sys/types.h>

#include "pdb.h"

int free_field(field_t *pntr)
{
    if(pntr->field_entry != NULL)
        free(pntr->field_entry);
    if(pntr->field_name != NULL)
        free(pntr->field_name);
    free(pntr);
    return 1;
}

void free_table(table_t *t)
{
    unsigned int i;
    field_t **curr;
    field_t *field;
    fprintf(stdout, "Freeing table %u\n", t->table_id);
    if(t->field_count > 0)
        for(curr = &t->fields; *curr != NULL;){
            field = *curr;
            *curr = field->next;
            fprintf(stdout, "[Table %u]: Freeing field entry %u\n", t->table_id,
                    field->field_id);
            free_field(field);
        }
    if(t->format != NULL){
        for(i = 0; i < t->format->max_fields; i++)
            free(t->format->field_format[i]);
        free(t->format->field_format); 
        free(t->format);
    }
    free(t);
    return;
}

field_t *alloc_field(const char *field_entry, unsigned int type, table_t *t)
{
    field_t *pntr;
    if(field_entry == NULL){
        fprintf(stderr, "Refusing to add NULL entry\n");
        return NULL;
    }
    pntr = malloc(sizeof(field_t));

    pntr->field_entry = malloc(strlen(field_entry) + 1);
    strncpy(pntr->field_entry, field_entry, strlen(field_entry) + 1);

    pntr->field_name = malloc(strlen(t->format->field_format[type] + 1));
    strncpy(pntr->field_name, t->format->field_format[type], 
            strlen(t->format->field_format[type]) + 1);

    pntr->type = type;
    pntr->next = NULL;
    pntr->field_id = t->field_count++;
    pntr->parent_table = t->table_id;

    fprintf(stdout, "[Table %u]: Allocated field %u with name:entry -> '%s: %s'\n",
            t->table_id, pntr->field_id, pntr->field_name, pntr->field_entry);
    return pntr;
}

field_format_t *alloc_field_format(unsigned int max_fields)
{
    field_format_t *pntr;
    pntr = malloc(sizeof(field_format_t));
    pntr->max_fields = max_fields;
    pntr->field_format = malloc(sizeof(char *) * max_fields);
    return pntr;
}

table_t *alloc_table(unsigned int id)
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

int read_file_format(char *format_string, table_t *t)
{
    char *token;
    char *copy;
    unsigned int i;
    unsigned int num_fields;

    copy = malloc(strlen(format_string) + 1);
    strncpy(copy, format_string, strlen(format_string) + 1);

    token = strtok(copy, "|");
    num_fields = atoi(token);
    t->format = alloc_field_format(num_fields);

    fprintf(stdout, "Number of fields: %u\n", t->format->max_fields);
    for(i = 0; i < num_fields; i++){
        token = strtok(NULL, "|");
        t->format->field_format[i] = malloc(sizeof(char) * strlen(token) + 1);
        strncpy(t->format->field_format[i], token, strlen(token) + 1);
        fprintf(stdout, "[%u] %s\n", i, t->format->field_format[i]);
    }
    fprintf(stdout, "Got field format\n");
    free(copy);
    return 0;
}

int parse_input_file(const char *file_path, table_t *t)
{
    int input_fd;
    char *copy;
    char *token;
    char *data_pntr;
    struct stat st;
    off_t file_size = 0;
    unsigned int i = 0;

    fprintf(stdout, "Opening file: %s\n", file_path);

    if((input_fd = open(file_path, O_RDONLY)) == -1){
        perror("open");
        return -1;
    }
    if(fstat(input_fd, &st) < 0){
        close(input_fd);
        perror("stat");
        return -1;
    }
    file_size = st.st_size;
    fprintf(stdout, "File size: %jd bytes\n", file_size);
    if((data_pntr = mmap(NULL, file_size, PROT_READ, 
                    MAP_PRIVATE | MAP_POPULATE, input_fd, 0)) == MAP_FAILED){
        close(input_fd);
        perror("mmap");
        return -1;
    }
    if(data_pntr == NULL){
        close(input_fd);
        perror("mmap");
        return -1;
    }

    close(input_fd);
    copy = malloc(file_size + 1);
    strncpy(copy, data_pntr, file_size + 1);
    munmap(data_pntr, file_size);

    read_file_format(copy, t);
    token = strtok(copy, "|");

    while(token != NULL){
        token = strtok(NULL, "|");
        fprintf(stdout, "[%u] %s: %s\n", i, t->format->field_format[i], token);
        if(i == t->format->max_fields - 1){
            fprintf(stdout, "\n");
            token = strtok(NULL, "|");
            i = 0;
            continue;
        }
        if(token == NULL)
            break;
        i++;
    }
    free(copy);
    return 0;
}

int main(int argc, char **argv)
{
    table_t *table = NULL;
    table = alloc_table(1);
    if(parse_input_file(argv[1], table) == -1)
        return -1;
    free_table(table);
    return 0;
}
