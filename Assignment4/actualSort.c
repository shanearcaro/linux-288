#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int leastSignificantBit(float number) {
    unsigned int b;
    union ufi {
        float f;
        int i;
    } u;
    u.f = number;
    b = u.i & 0x1;
}

int nthBit(int number, int n) {
    return (number >> n) & 1;
}

void printArray(char* description, float arr[], int length) {
    for (int i = 0; i < length; i++) {
        printf("%s - [%d]: %f\n", description, i, arr[i]);
    }
    printf("\n");
}

void reverseArray(float *array, int size) {
    for (int i = 0; i < (size / 2); i++) {
        float swap = array[size - 1 - i];
        array[size - 1 - i] = array[i];
        array[i] = swap;
    }
}

void radixsort(float *array, unsigned int length, unsigned int bits) {
    float sum = 0;
    // Create buckets
    float *bucketsA = (float *) malloc(length * sizeof(float));
    float *bucketsB = (float *) malloc(length * sizeof(float));

    unsigned int aIndex = 0, bIndex = 0;

    for (int i = 0; i < bits; i++) {
        /* Sort array using digit position d as the key. */
        for (int j = 0; j < length; j++) {
            if (i == 0) {
                printf("Val: %f\n", array[j]);
                sum += array[j];
            }
            unsigned int conversion = * (unsigned int *) &array[j];
            int positionBit = nthBit(conversion, i);
            // printf("Number: %f, Position Bit: %d\n", array[j], positionBit);
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
        // printf("\n");
        // printArray("A", bucketsA, length);
        // printArray("B", bucketsB, length);

        // Reset the memory of the buckets
        memset(&bucketsA[0], 0, sizeof(float) * length);
        memset(&bucketsB[0], 0, sizeof(float) * length);

        // Reset bucket index
        aIndex = bIndex = 0;

        /**
         * When bit is equal to 31
         * Flip the order of bucketB
         * Put bucketB infront of bucketA instead
         */
    }
    printf("Total: %f\n", sum);
}

int main(int argc, char* argv[]) {
    float arr[] = {10, 20, 30, 100, 34234.234, -2342.23423, 0.234234, -123123.23423, 234.3432, 467456.4235, -67651.4352345, 6475.245, -12349.1324, 8764, -7250};
    radixsort(arr, sizeof(arr) / sizeof(arr[0]), 32);
    // reverseArray(arr, 3);

    printArray("Final", arr, sizeof(arr) / sizeof(arr[0]));
}