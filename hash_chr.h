#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// hash table entry structure
typedef struct hash_entry {
    char *value;
    int visits;
} hash_entry;

// hash table structure
typedef struct hash_table {
    int size;
    int num_entries;
    hash_entry **entries;
    int *taken;
} hash_table;

hash_table *hash_init(int size);

int hash_insert(hash_table *table, char *value);

int hash_remove(hash_table *table, char *value);

int hash_find(hash_table *table, char *value);

void hash_destroy(hash_table *table);