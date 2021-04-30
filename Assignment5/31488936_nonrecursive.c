/**
 * Now that I've finished this program, it's important to note that the q_closed should not be a queue.
 * I left it as a queue in the beginning because I initially was going to take a different approach on
 * how I traversed the directory tree by following BFS more strictly. I found that just popping and
 * pushing directories into open while only pushing files into closed was a much better approach than BFS. Because
 * the q_closed files are never popped and because it is eventually sorted into an array anyway, so that it
 * can be sorted within a reasonable time, it should not be a queue but rather an array. I'm not going to try
 * and fix this now as the code has already been written and it passes all checks but this code should be modified.
 */

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

/**
 * File struct to combine elements of files together
 */
struct File {
    char* filePath;
    int fileSize;
    int directory;
};

/**
 * Queue struct, basically a doubly linked list
 */
struct Queue {
    struct File file;
    struct Queue* next;
    struct Queue* previous;
};

struct Queue* q_findLast(struct Queue** head);
struct Queue* q_peekIndex(struct Queue** head, int index);
struct Queue* q_pop(struct Queue** head);
struct Queue* q_peek(struct Queue** head);
void q_push(struct Queue** head, struct File file);
void q_print(struct Queue** head);
int q_size(struct Queue** head);
void q_sort(struct Queue** head);

int openIndex = 0;
int closedIndex = 0;
int searchedIndex = 0;
int addedDirectories = 0;

struct Queue* q_open = NULL;
struct Queue* q_closed = NULL;

void search(char* path);
struct File getInfo(char* path);
void sort(struct Queue** head);

int main(int argc, char* argv[]) {
    // Check for file path argument
    if (argc != 2) {
        printf("Invalid number of arguments.\n");
        exit(EXIT_FAILURE);
    }
    search(argv[1]);
    sort(&q_closed);
}

/**
 * Add new value into Queue
 */
void q_push(struct Queue** head, struct File file) {
    struct Queue* current = (struct Queue*) malloc(sizeof(struct Queue));
    current->file = file;

    if ((*head) == NULL) {
        *head = current;
    }
    else {
        struct Queue* last = q_findLast(head);
        last->next = current;
        current->previous = last;
    }
}

/**
 * Pop oldest value placed into Queue, FIFO
 */
struct Queue* q_pop(struct Queue** head) {
    struct Queue* current;
    if ((*head) == NULL)
        return NULL;

    if ((*head)->next == NULL) {
        current = (*head);
        (*head) = NULL;
        return current;
    }

    current = (*head);
    (*head) = (*head)->next;
    (*head)->previous = NULL;
    return current;
}

/**
 * Look at the head value without deleting it
 */
struct Queue* q_peek(struct Queue** head) {
    if ((*head) == NULL)
        return NULL;
    return (*head);
}

/**
 *  Look at a specific index without deleting it
 */
struct Queue* q_peekIndex(struct Queue** head, int index) {
    if (index > q_size(head) || index < 0)
        return NULL;

    if ((*head) == NULL)
        return NULL;

    struct Queue* current = (*head);

    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
}

/**
 * Helper function used to add new elements to the end of the Queue
 */
struct Queue* q_findLast(struct Queue** head) {
    struct Queue* current = (*head);
    
    if (current == NULL)
        return NULL;

    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

/**
 * Print out the queue in the desired format
 */
void q_print(struct Queue** head) {
   struct Queue* current = (*head);

   while (current != NULL) {
       printf("%d\t%s\n", current->file.fileSize, current->file.filePath);
       current = current->next;
   } 
}

/**
 * Find the length of a Queue
 */
int q_size(struct Queue** head) {
    struct Queue* current = (*head);
    int length = 0;

    while (current != NULL) {
        length++;
        current = current->next;
    }
    return length;
}

void search(char* path) {
    DIR* dir;
    struct dirent* dirEntry;
    char* fileName;

    // Add the starting directory to the Queue
    struct File root = getInfo(path);
    q_push(&q_open, root);

    while (q_size(&q_open) != 0) {
        // Open directory
        dir = opendir(q_peek(&q_open)->file.filePath);

        // Read all elements within a directory
        for (;;) {
            dirEntry = readdir(dir);

            // Check for end of file
            if (dirEntry == NULL)
                break;

            // Ignore previous and current directory and continue searching on all sub elements
            if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) {
                fileName = malloc(strlen(q_peek(&q_open)->file.filePath) + strlen(dirEntry->d_name) + 2);
                sprintf(fileName, "%s/%s", q_peek(&q_open)->file.filePath, dirEntry->d_name);
                
                // Turn the current searched path into a file
                struct File current = getInfo(fileName);

                // If current is a directory add to open, otherwise add to closed
                if (current.directory == 0) 
                    q_push(&q_open, current);
                else if (current.directory == 1)
                    q_push(&q_closed, current);
            }
        }
        // All elements within the previous directory have been searched at this point

        // Close the previous directory
        closedir(dir);

        // Remove previous directory from open Queue
        q_pop(&q_open);
    }
}

struct File getInfo(char* path) {
    struct File file;
    struct stat status;
    mode_t mode;
    int value;

    // Check stats of path and quit if it fails
    value = lstat(path, &status);
    if (value == -1) {
        printf("Cannot read %s\n", path);
        struct File error;
        return error;
    }

    // Determine if path is file or directory
    mode = status.st_mode;
    
    file.filePath = path;
    file.fileSize = status.st_size;
    if (S_ISDIR(mode))
        file.directory = 0;
    else if (S_ISREG(mode))
        file.directory = 1;
    else
        file.directory = -1;

    return file;
}

/**
 * Sort searched files by their size
 * 
 * This function works as intended but is pretty slow because of q_peekIndex
 * Having to iterate through the queue every time by it's next pointer just to
 * find an element makes the sorting incredibly slow.
 */
void q_sort(struct Queue** head) {
    int size = q_size(head);

    // Queue is already sorted with 0 or 1 elements
    if (size <= 1)
        return;

    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            // Flip file spots if first file is bigger than second
            struct Queue* first = q_peekIndex(head, i);
            struct Queue* second = q_peekIndex(head, j);

            if (first->file.fileSize > second->file.fileSize) {
                struct File temp = first->file;
                first->file = second->file;
                second->file = temp;
            }
        }
    }
}

/**
 * Faster way to sort queue is by changing the queue into an array of files
 * The files can be directly compared - sorts incredibly fast compared to q_sort
 */
void sort(struct Queue** head) {
    int size = q_size(head);

    struct File* files = malloc(sizeof(struct File) * size);
    struct Queue* current = (*head);

    // Fill array of files
    for (int i = 0; i < size; i++) {
        files[i] = current->file;
        current = current->next;
    }

    // Sort array of files
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            // Flip file spots if first file is bigger than second
            if (files[i].fileSize > files[j].fileSize) {
                struct File temp = files[i];
                files[i] = files[j];
                files[j] = temp;
            }
        }
    }

    // Just print the values here at this point
    for (int i = 0; i < size; i++) {
        printf("%d\t%s\n", files[i].fileSize, files[i].filePath);
    }
}
