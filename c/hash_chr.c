#include "hash_chr.h"

// initialize hash table
hash_table *hash_init(int size) {
    printf("initializing hash table with size: %d\n", size);

    if (size <= 0) return NULL;

    // allocate hash table
    hash_table *table = malloc(sizeof(hash_table));
    if (table == NULL) return NULL;

    // allocate list of entries in hash table
    table->entries = malloc(sizeof(hash_entry*) * size);
    if (table->entries == NULL) return NULL;

    // initialize all entries
    for (int i=0; i < size; i++) table->entries[i] = NULL;

    table->size = size;
    table->num_entries = 0;

    // initialize tracking array to make table iterable
    table->taken = malloc(size * sizeof(int));
    if (table->taken == NULL) {
        printf("memory allocation failed");
        exit(1);
    }

    return table;
}

// sum ASCII value of every character in word
// function name is like long-ord for the ord 
// function in higher level langs
int lord(char *word) {
    int wordVal = 0;
    int i = 0;
    while (word[i] != '\0') {
        wordVal += word[i++];
    }
    return wordVal;
}

// general hash function 
int hash(char *word) {
    return lord(word) - 65; // moves 'A' to 0 index
}

// insert value into hash table function
int hash_insert(hash_table *table, char *value) {
    if (table == NULL) return -1;
    // helper vars
    int rounds = 0; // keeps track of num times hash is probed
    int end_rounds = 0; // tells function to start at max hash value
    int end = table->size; // end of hash table

    int i = hash(value); // initial index to check
    int hash_val = hash(value); // same but saved for later
    int new_i = hash(value); // used as next "pointer"
    hash_entry *entry = table->entries[i]; // initial try
    // while the next spot isn't occupied
    while (entry) {
        // return if value already in hash table
        if (strcmp(entry->value, value) == 0) {
            return 0;
        } else {
            // visited, increment by 12135 (max lord value)
            entry->visits += 1;
            new_i += 12135;
            // after exceeding size of table, start at max lord value
            // and increment down by hash value (more spots that won't
            // be as full)
            if (new_i > end) {
                rounds += 1;
                new_i = 12135 - (rounds * hash_val);
            }
            // after passing 0 on the turn around, just start at
            // max lord value and increment down by 1
            if (new_i < 0 && end_rounds == 0) {
                end_rounds = rounds;
                new_i = 12135 - (rounds - end_rounds);
            } else if (new_i < 0) {
                new_i = 12135 - (rounds - end_rounds);
            }
            i = new_i;
            entry = table->entries[i];
        }
    }

    // if value not in hash table, make new entry
    hash_entry *new_entry = malloc(sizeof(hash_entry));

    new_entry->value = value;
    new_entry->visits = 0;
    table->entries[i] = new_entry;
    table->taken[table->num_entries] = i;
    table->num_entries += 1;

    return 0;
}

// remove value from hash table
int hash_remove(hash_table *table, char *value) {
    if (table == NULL) return -1;
    int rounds = 0;
    int end_rounds = 0;
    int end = table->size;

    int i = hash(value);
    int hash_val = hash(value);
    int new_i = hash(value);
    hash_entry *entry = table->entries[i];
    // similar logic to insert, follows same path word in
    // question would have followed on insert
    while (entry) {
        if (strcmp(entry->value, value) != 0) {
            if (entry->visits > 0) {
                new_i += 12135;
                if (new_i > end) {
                    rounds += 1;
                    new_i = 12135 - (rounds * hash_val);
                } 
                if (new_i < 0 && end_rounds == 0) {
                    end_rounds = rounds;
                    new_i = 12135 - (rounds - end_rounds);
                } else if ( new_i < 0) {
                    new_i = 12135 - (rounds - end_rounds);
                }
                i = new_i;
                entry = table->entries[i]; 
            } else {
                break;
            }
        } else {
            table->entries[i] = NULL;
            printf("%s found, removed\n", value);
            return 0;
        } 
    }
    printf("word not found %s\n", value);
    return -1;
}


// finding a value in hash table
int hash_find(hash_table *table, char *value) {
    if (table == NULL) return -1;
    int rounds = 0;
    int end_rounds = 0;
    int end = table->size;

    int i = hash(value);
    int hash_val = hash(value);
    int new_i = hash(value);
    hash_entry *entry = table->entries[i];
    // similar to insert and remove, just built to print output
    // on finding/not finding the value in question
    while (entry) {
        if (strcmp(entry->value, value) == 0) {
            printf("word %s\t\tfound\n", value);
            printf("hash:\t\t%d\n", i);
            printf("visits:\t\t%d\n", entry->visits);
            return 0;
        } else if (entry->visits == 0) {
            break;
        } else {
            new_i += 12135;
            if (new_i > end) {
                rounds += 1;
                new_i = 12135 - (rounds * hash_val);
            } 
            if (new_i < 0 && end_rounds == 0) {
                end_rounds = rounds;
                new_i = 12135 - (rounds - end_rounds);
            } else if (new_i < 0) {
                new_i = 12135 - (rounds - end_rounds);
            }
            i = new_i;
            entry = table->entries[i];
        }
    }
    printf("word '%s'\tnot found\n", value);
    return -1;
}

// function to deallocate table after use
void hash_destroy(hash_table *table) {
    printf("destroying hash table of size %d\n", table->size);
    if (table == NULL) return;

    for (int i=0; i < table->num_entries; i++) {
        int index = table->taken[i];
        hash_entry *entry = table->entries[index];
        free(entry);
    }

    free(table);
}
