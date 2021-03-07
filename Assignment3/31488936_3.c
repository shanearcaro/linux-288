#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char** insertion sorter
void insertionSort(char* var[], int size);

int main(int argc, char* arv[], char* envp[]) {
    int index = 0;

    // Can't tell size of array so just iterator through with counter
    while (envp[index])
        index++;
    insertionSort(envp, index);

    // Print out data in order
    for (int i = 0; i < index; i++)
        printf("%s\n", envp[i]);
    printf("\n");
}

/**
 * Insertion Sort for char** of elements
 */
void insertionSort(char* var[], int size) {
    // Delimiter character used for strtok
    const char* delimiter = "=";

    // Previous index used for comparison
    int previous;

    // Loop through entire array starting at one so previous position is in bounds
    for (int index = 1; index < size; index++) {
        previous = index - 1;
        char* target = var[index];

        // Compare current index to previous and switch if not in order
        do {
            // Have to create copies of data because strtok alters originals
            char* current = calloc(strlen(var[previous + 1]) + 1, sizeof(char));
            char* before = calloc(strlen(var[previous]) + 1, sizeof(char));

            // Putting original data in copy
            strcpy(current, var[previous + 1]);
            strcpy(before, var[previous]);

            // Compare data to see if its in order
            int comparison = strcmp(strtok(current, delimiter), strtok(before, delimiter));

            // If comparison is >= 0 then data is in correct order so break
            if (comparison >= 0)
                break;

            // Swtich data positions
            var[previous + 1] = var[previous];
            var[previous] = target;
            target = var[previous];
            previous--;

            // Free pointer memory
            free(current);
            free(before);
        
        // Make sure previous doesn't go out of bounds
        } while(previous >= 0);
    }
}
