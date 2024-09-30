
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "hash_chr.h"

#define MAX_WORDS 100000
#define MAX_WORD_LENGTH 100

char word[MAX_WORD_LENGTH];
char *words[MAX_WORDS];
int wordCount = 0;

// printing arrays
void printArray(char **arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%s\n", arr[i]);
    }
}

// safer version of strcpy, checks memory first
void sstrcpy(char** arr, char *new_item, int index) {
    arr[index] = malloc(strlen(new_item) + 1);
    if (arr[index] == NULL) {
        perror("Memory allocation failed");
        return;
    }
    strcpy(arr[index], new_item);
}

// grabs alphabetic characters only
void cleanWord(char *word) {
    int i = 0, j = 0;
    while (word[i]) {
        if (isalpha(word[i])) {
            word[j++] = word[i];
        }
        i++;
    }
    word[j] = '\0';
}

// reading in txt file
void readFile(const char *filename) {
    // open file
    FILE *file;
    file = fopen(filename, "r");

    // Read words from the file one by one (separates words by white space)
    while (fscanf(file, "%s", word) == 1) { // read each word up to 99 characters long
        if (wordCount >= MAX_WORDS) {
            printf("Too many words in file.\n");
            break;
        }
        cleanWord(word);
        // only add words of alphabetic letters, the first character
        // of words without them will be '\0' after the cleanWord
        // function, so filter those out
        if (word[0] == '\0') {
            continue;
        } else {
            // dynamically allocate memory for each word and store it
            words[wordCount] = malloc(strlen(word) + 1);
            if (words[wordCount] == NULL) {
                perror("Memory alloction failed");
                return;
            }
            strcpy(words[wordCount], word);
            wordCount++;
        }
    }
    fclose(file);
}

// preprocess the file
long getFileSize(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    // move file pointer to end of the file
    fseek(file, 0, SEEK_END);
    // get current position of the file pointer, == size of file
    long size = ftell(file);
    // close the file
    fclose(file);

    return size;
}

/// quick sort stuff
// swapping two strings in array
void swap(char** a, char** b) {
    char* temp = *a;
    *a = *b;
    *b = temp;
}
// parition subrouting used for quick sort
int partition(char** arr, int low, int high) {
    // pointer to initial pivot, smaller element
    char* pivot = arr[high];
    int i = low - 1;
    // loop through to end, swapping elements along
    // the way if less than pivot
    for (int j = low; j < high; j++) {
        if (strcmp(arr[j], pivot) < 0) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    // move pivot to where it should be to partition
    // the list where everything < pivot is left of it
    // and everything > pivot is right of it
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// main in place quick sort function via partition method
void quickSort(char** arr, int low, int high) {
    if (low < high) {
        // partition array into two sublists based on
        // last element
        int part = partition(arr, low, high);
        // recursively sort sublists until sorted
        quickSort(arr, low, part - 1);
        quickSort(arr, part + 1, high);
    }
}
/// end quick sort stuff

/// merge sort stuff
// merge two halves
void merge(char** arr, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1; // left sub array
    int n2 = right - mid; // right sub array
    // temp arrays for subs
    char *L[n1], *R[n2];
    // copy words into sub temp arrays
    for (i = 0; i < n1; i++) {
        sstrcpy(L, arr[left + i], left + i);
    }
    for (j = 0; j < n2; j++) {
        sstrcpy(R, arr[mid + 1 + j], mid + 1 + j);
    }

    // merge temp arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (strcmp(L[i], R[j]) <= 0) {
            // copy L[i] into arr[k]
            sstrcpy(arr, L[i], k);
            i++;
        } else {
            // copy R[j] into arr[k]
            sstrcpy(arr, R[j], k);
            j++;
        }
        k++;
    }

    // copy remaining elements of L[] and R[] if any
    while (i < n1) {
        sstrcpy(arr, L[i], k);
        i++;
        k++;
    }
    while (j < n2) {
        sstrcpy(arr, R[j], k);
        j++;
        k++;
    }
}
// main merge sort func
void mergeSort(char** arr, int left, int right) {
    if (left < right) {
        // find middle point
        int mid = left + (right - left) / 2;

        // sort left and right of mid
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // merge sorted sub arrays
        merge(arr, left, mid, right);
    }
}
//// end merge sort stuff

// displaying help function
void print_help() {
    printf("Usage sort_ [-h] [-u] [-d] [-R] filename {merge,quick}\n");
    printf("(heap, radix coming soon)\n");
    printf("Sort App\n\n");
    printf("positional arguments:\n");
    printf("  filename\t\t\t.txt file to be sorted\n");
    printf("  {merge,quick}\t\t\tsort method\n\n");
    printf("options:\n");
    printf("  -h, --help\t\t\tshow this help message and exit\n");
    printf("  -u, --uniq\t\t\tshow unique words\n");
    printf("  -d, --desc\t\t\tsort in descending order\n");
    printf("  -R, -random-sort\t\t(also coming soon) sort by a random order\n");
}

int main(int argc, char *argv[]) {
    char *filename;
    // set the defaults
    char *sort_type = "quick";
    bool uniq = false;
    bool desc = false;
    bool rand = false;
    
    if (argc > 1){
        // display help option if first argument
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_help();
            return 0;
        } else {
            filename = argv[1];
            struct stat buffer;

            // verify the file exists
            if (stat(filename, &buffer) != 0) {
                printf("no file at %s\n", filename);
                return 1;
            }
        }
    }
    // if there are more arguments passed
    if (argc > 2) {
        // parse the args
        for (int i=2; i < argc; i++) {
            if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--uniq") == 0) {
                uniq = true;
            }
            if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--desc") == 0) {
                desc = true;
            }
            if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "-random-sort") == 0) {
                rand = true;
            }
            if (strcmp(argv[i], "quick") == 0) {
                sort_type = "quick";
            }
        }
    }
    if (filename == NULL) {
        printf("no filename passed\n");
        return 1;
    } 

    printf("sort tool ready to run w/ following params\n");
    printf("filename:\t%s\n", filename);
    printf("sort_type:\t%s\n", sort_type);
    printf("uniq:\t\t%d\n", uniq);
    printf("desc\t\t%d\n", desc);
    printf("rand\t\t%d\n", rand);

    // read in words from the file
    readFile(filename);

    // final_words variables
    char *final_words[MAX_WORDS];
    int final_count = 0;

    // make unique group if passed, else just copy words over
    if (uniq) {
        printf("making unique word set\n");
        hash_table *words_set = hash_init(MAX_WORDS);

        for (int i=0; i < wordCount; i++) {
            char *word = words[i];
            hash_insert(words_set, word);
        }
        printf("set successfully made\n");

        // iterate through hash table to get unique words to sort
        int curr_size = words_set->num_entries;
        int index;
        hash_entry *curr;
        char *curr_word;
        printf("num_entries:\t%d\n", curr_size);
        for (int i=0; i < curr_size; i++) {
            index = words_set->taken[i];
            curr = words_set->entries[index];
            curr_word = curr->value;
            sstrcpy(final_words, curr_word, i);
            final_count++;
        }
        hash_destroy(words_set);
        printf("words read in, hash table destroyed");
    } else {
        for (int i=0; i < wordCount; i++) {
            sstrcpy(final_words, words[i], i);
            final_count++;
        }
    }

    printf("\nSorted words in the file:\n");
    printf("final_count\t%d\n", final_count);

    // choose sort type
    if (strcmp(sort_type, "quick") == 0){
        quickSort(final_words, 0, final_count - 1);
    } else if (strcmp(sort_type, "merge") == 0) {
        mergeSort(final_words, 0, final_count - 1);
    }
    
    // if descending order is requested
    if (desc) {
        int end = final_count / 2 - 1;
        int j = final_count - 1;
        for (int i=0; i < end; i++) {
            swap(&final_words[i], &final_words[j]);
            j--;
        }
    }
    printArray(final_words, final_count);
}
