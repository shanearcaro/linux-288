#include <stdio.h>
#include <stdlib.h>

// Global variable of length of an unsigned int
int length = sizeof(unsigned int) * 8;

// Convert decimal number to binary
void decimalToBinary(unsigned int number, char* binary);

// Convert binary to decimal number: long long so not negative rounding issues for large numbers
long long binaryToDecimal(char* binary);

// Invert the bits of a binary number
void inversion(char* binary);

// Print char array as binary with gaps
void print(char* binary);

// Power of a number
long long power(int a, int b);

int main(int argc, char* argv[]) {
    // If a single argument isn't given exit
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    // Convert argument passed to int
    unsigned int number = atoi((argv[1]));
    char *binary = malloc(length);

    // Print number and convert to binary
    printf("%-15d: ", number);
    decimalToBinary(number, binary);

    // Print
    print(binary);
    inversion(binary);
    free(binary);
}

/**
 * Convert decimal to binary number
*/
void decimalToBinary(unsigned int number, char* binary) {
    // Convert decimal to binary
    int index = length - 1;
    while (number > 0) {
        binary[index] = number & 0x1;
        number >>= 1;
        index--;
    }
}

/**
 * Convery binary to decimal number 
*/
long long binaryToDecimal(char* binary) {
    unsigned int total = 0;
    int p = 0;

    for (int i = length - 1; i >= 0; i--) {
        if (binary[i] == 1)
            total += power(2, p);
        p++;
    }
    return total;
}

/**
 * Inverts bits of binary number
 * Tried a different method with bitshift
 * but couldn't get the logic to work properly
*/
void inversion(char* binary) {
    char* invertBinary = malloc(length);

    // Simple array to switch bit positions
    for (int i = 0; i < length; i++) 
        invertBinary[i] = binary[(length - 1) - i];

    printf("%-15lld: ", binaryToDecimal(invertBinary));
    print(invertBinary);
    free(invertBinary);
}

/**
 * Print binary version of char array
 */
void print(char* binary) {
    for (int i = 0; i < length; i++) {
        printf("%d", binary[i]);
        if ((i + 1) % 4 == 0)
            printf(" ");
    }
    printf("\n");
}

/**
 * I tried using the math.h power function here but there was a problem with the
 * linker detecting the library. I included the library in the command but I was
 * still having problems with it. Because a power function is super simple I 
 * just made my own and used it here.
*/
long long power(int a, int b) {
    if (b <= 1)
        return a;
    return a * power(a, b - 1);
}
