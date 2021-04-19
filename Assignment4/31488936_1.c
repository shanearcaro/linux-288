#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

int nthBit(int number, int n) {
    return (number >> n) & 1;
}

void reverseArray(float *array, int size) {
    for (int i = 0; i < (size / 2); i++) {
        float swap = array[size - 1 - i];
        array[size - 1 - i] = array[i];
        array[i] = swap;
    }
}

void radixsort(float *array, unsigned int length, unsigned int bits) {
    double sum = 0;
    // Create buckets
    float *bucketsA = (float *) malloc(length * sizeof(float));
    float *bucketsB = (float *) malloc(length * sizeof(float));

    unsigned int aIndex = 0, bIndex = 0;

    for (int i = 0; i < bits; i++) {
        // Sort array using digit position d as the key.
        for (int j = 0; j < length; j++) {
            if (i == 0) sum += array[j];
            unsigned int conversion = * (unsigned int *) &array[j];
            int positionBit = nthBit(conversion, i);
            if (positionBit == 0) {
                bucketsA[aIndex] = array[j];
                aIndex++;
            }
            else {
                bucketsB[bIndex] = array[j];
                bIndex++;
            }
        }
        // Combine and move sorted buckets into original array
        if(i == bits - 1) {
            reverseArray(bucketsB, bIndex);
            memcpy(array,          bucketsB, (bIndex + 1) * sizeof(float));
            memcpy(array + bIndex, bucketsA, (aIndex + 1) * sizeof(float));
        }
        else {
            memcpy(array,           bucketsA, (aIndex + 1) * sizeof(float));
            memcpy(array + aIndex,  bucketsB, (bIndex + 1) * sizeof(float));
        }

        // Reset the memory of the buckets
        memset(&bucketsA[0], 0, sizeof(float) * length);
        memset(&bucketsB[0], 0, sizeof(float) * length);

        // Reset bucket index
        aIndex = bIndex = 0;
    }
    printf("Total: %f\n", sum);

    // Free reserved memory
    free(bucketsA);
    free(bucketsB);
}

int main(int argc, char* argv[]) {
    // Exit if single file path isn't provided
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    char* filePath = argv[1];
    printf("File Path: %s\n", filePath);

    int i, fd;
    float *map;
    int fileSize, count;
    struct stat st;

    // Open file and get file size
    fd = open(filePath, O_RDWR | O_CREAT);
    fstat(fd, &st);
    fileSize = st.st_size;
    count = fileSize / sizeof(float);

    printf("File size: %d\n", fileSize);

    // Exit if file can't be opened
    if (fd == -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    
    // Map the file to virtual memory
    map = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Just need to sort these stupid numbers
    radixsort(map, count, 32);

    // Exit if file can't be mapped
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mapping the file");
        exit(EXIT_FAILURE);
    }

    // Unmap the file
    if (munmap(map, fileSize) == -1) {
        perror("Error un-mapping the file");
        // Decide here whether to close (fd) and exit() or not. Depends... <----
    }
    close(fd);
    return 0;
}