#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILES 10000

struct File* filesPtr;
int filesIndex;

struct File {
    char* filePath;
    int fileSize;
};

void processDirectory(char* path);
void search(char* path);
void sort(struct File* files);

int main(int argc, char* argv[]) {
    // Check for file path argument
    if (argc != 2) {
        printf("Invalid number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    // Create files array and pointer to start of memory
    struct File files[MAX_FILES];
    filesPtr = files;
    filesIndex = 0;

    // Search at user specifined path
    search(argv[1]);

    // Reset pointer memory and sort files
    filesPtr = files;
    sort(filesPtr);

    // Print sorted files
    for (int i = 0; i < filesIndex; i++) {
        printf("%d\t%s\n", filesPtr->fileSize, filesPtr->filePath);
        filesPtr++;
    }
}

/**
 * User search to recursiverly look through a directory for all files and sub-directories
 */
void processDirectory(char* path) {
    DIR* dir;
    struct dirent* dirEntry;
    char* fileName;

    // Open directory
    dir = opendir(path);

    // Read all elements within a directory
    for (;;) {
        dirEntry = readdir(dir);
        // Check for end of file
        if (dirEntry == NULL)
            break;
        // Ignore previous and current directory and continue searching on all sub elements
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) {
            fileName = malloc(strlen(path) + strlen(dirEntry->d_name) + 2);
            sprintf(fileName, "%s/%s", path, dirEntry->d_name);
            search(fileName);
        }
    }
    // Close directory
    closedir(dir);
}

/**
 * Analyze directory and files
 */
void search(char* path) {
    struct stat status;
    mode_t mode;
    int value;

    // Check stats of path and quit if it fails
    value = lstat(path, &status);
    if (value == -1) {
        printf("Cannot read %s\n", path);
        return;
    }

    // Determine if path is file or directory
    mode = status.st_mode;
    // If path is direcotry continue searching
    if (S_ISDIR(mode)) {
        processDirectory(path);
    }
    // If path is file add to files array
    else if (S_ISREG(mode)) {
        (filesPtr + filesIndex)->filePath = path;
        (filesPtr + filesIndex)->fileSize = status.st_size;
        filesIndex++;
    }
}

/**
 * Sort searched files by their size
 */
void sort(struct File* files) {
    for (int i = 0; i < filesIndex; i++) {
        for (int j = i + 1; j < filesIndex; j++) {
            // Flip file spots if first file is bigger than second
            if (files[i].fileSize > files[j].fileSize) {
                struct File temp = files[i];
                files[i] = files[j];
                files[j] = temp;
            }
        }
    }
}
