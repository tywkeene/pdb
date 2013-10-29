#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/mman.h> 
#include <sys/types.h>

#include "pdb.h"

int free_table_entry(table_entry_t *pntr)
{
    if(pntr->entry_data != NULL)
        free(pntr->entry_data);
    if(pntr->entry_name != NULL)
        free(pntr->entry_name);
    free(pntr);
    return 1;
}

void free_table(table_t *t)
{
    unsigned int i;
    fprintf(stdout, "Freeing table %u\n", t->table_id);
    if(t->entries != NULL){
        for(i = 0; i < t->format->max_entries; i++){
            fprintf(stdout, "Freeing table entry %u\n", i);
            free(t->entries[i]->entry_name);
            free(t->entries[i]->entry_data);
            free(t->entries[i]);
        }
        free(t->entries);
    }
    if(t->format != NULL){
        for(i = 0; i < t->format->max_entries; i++)
            free(t->format->entry_format[i]);
        free(t->format->entry_format); 
        free(t->format);
    }
    free(t);
    return;
}

table_entry_t *alloc_table_entry(const char *entry, unsigned int type, table_t *t)
{
    table_entry_t *pntr;
    if(entry == NULL){
        fprintf(stderr, "Refusing to add NULL entry\n");
        return NULL;
    }
    pntr = malloc(sizeof(table_entry_t));

    pntr->entry_data = malloc(strlen(entry) + 1);
    strncpy(pntr->entry_data, entry, strlen(entry) + 1);

    pntr->entry_name = malloc(strlen(t->format->entry_format[type]) + 1);
    strncpy(pntr->entry_name, t->format->entry_format[type], 
            strlen(t->format->entry_format[type]) + 1);

    pntr->type = type;
    pntr->table_entry_id = t->entry_count++;
    pntr->parent_table = t->table_id;

    return pntr;
}

entry_format_t *alloc_entry_format(unsigned int max_entries)
{
    entry_format_t *pntr;
    pntr = malloc(sizeof(entry_format_t));
    pntr->max_entries = max_entries;
    pntr->entry_format = malloc(sizeof(char *) * max_entries);
    return pntr;
}

table_t *alloc_table(unsigned int id)
{
    table_t *pntr;
    pntr = malloc(sizeof(table_t));
    pntr->table_id = id; 
    pntr->entry_count = 0;
    pntr->entries = NULL;
    pntr->format = NULL;
    pntr->entries = NULL;
    fprintf(stdout, "Allocated table %u\n", id);
    return pntr;
}

unsigned int count_token(char *string, const char *token_string)
{
    unsigned int count = 0;
    char *token;
    char *copy;

    copy = malloc(strlen(string) + 1);
    strncpy(copy, string, strlen(string) + 1);
    for(token = strtok(copy, token_string); token != NULL; 
            token = strtok(NULL, token_string))
        count++;
    free(copy);
    return count;
}

int read_file_format(char *format_string, table_t *t)
{
    char *token;
    char *copy;
    char *format_string_copy;
    unsigned int i;
    unsigned int num_entries;

    copy = malloc(strlen(format_string) + 1);
    strncpy(copy, format_string, strlen(format_string) + 1);
    format_string_copy = strtok(copy, "\r\n");

    num_entries = count_token(format_string_copy, "|");
    t->format = alloc_entry_format(num_entries);

    fprintf(stdout, "Number of entries: %u\n", t->format->max_entries);
    token = strtok(format_string_copy, "|");
    for(i = 0; i < num_entries; i++){
        t->format->entry_format[i] = malloc(sizeof(char) * strlen(token) + 1);
        strncpy(t->format->entry_format[i], token, strlen(token) + 1);
        fprintf(stdout, "[%u] %s\n", i, t->format->entry_format[i]);
        token = strtok(NULL, "|");
        if(token == NULL)
            break;
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
    t->entries = malloc(sizeof(table_entry_t *) * t->format->max_entries);
    token = strtok(copy, "|");

    while(token != NULL && i < t->format->max_entries){
        //fprintf(stdout, "[%u] %s: %s\n", i, t->format->entry_format[i], token);
        t->entries[i] = alloc_table_entry(token, i, t);
        if((token = strtok(NULL, "|")) == NULL)
            break;
        else
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
