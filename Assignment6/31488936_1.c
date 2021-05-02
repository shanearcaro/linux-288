#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <pthread.h>

struct Workers {
    int id;
    int start;
    int size;
};

float sum = 0;
float* map;

void* start(void* worker) {
    // Convert void* to a Workers
    struct Workers currentWorker = *(struct Workers *) worker;
    float partialSum = 0;
    printf("Thread[%d] starting work:\n", currentWorker.id);
    
    // Do the partial sum for each thread
    for (int i = currentWorker.start; i < currentWorker.start + currentWorker.size; i++) {
        float value = map[i];
        printf("\tThread[%d] adding value[%d]: %f\n", currentWorker.id, i, value);
        partialSum += value;
    }
    printf("Thread[%d] finished working, added partial sum: %f\n", currentWorker.id, partialSum);
    sum += partialSum;

    // Exit finished thread
    pthread_exit(NULL);
}

void getFile(char* path, int* length) {
    int fd;
    int fileSize, count;
    struct stat status;

    // Open file and get file size
    fd = open(path, O_RDWR | O_CREAT);

    if (fd == -1) 
        printf("Error opening file for reading\n");

    fstat(fd, &status);
    fileSize = status.st_size;
    *length = (fileSize / sizeof(float));

    // Map the file to virtual memory
    map = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void createWorkers(struct Workers* workers, int threadCount, int length) {
    int work = length / threadCount;
    int remainderWork = work;

    // Remainder set for last worker if needed
    if (length % threadCount != 0)
        remainderWork = work + length - (threadCount * work);

    // Set stats
    for (int i = 0; i < threadCount; i++) {
        workers[i].id = i;
        workers[i].size = (i != threadCount - 1) ? work : remainderWork;
        workers[i].start = i * work;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    // Take arguments from command line
    int threadCount = atoi(argv[1]);
    char* file = argv[2];

    int length = 0;
    getFile(file, &length);

    struct Workers* workers = (struct Workers*) malloc(sizeof(struct Workers) * threadCount);;
    createWorkers(workers, threadCount, length);

    for (int i = 0; i < threadCount; i++) {
        struct Workers currentWorker = workers[i];
        printf("Worker: ID[%d], Size[%d], Start[%d]\n", currentWorker.id, currentWorker.size, currentWorker.start);
    }

    // File couldn't be properly read, abandon
    if (map == NULL)
        return 1;

    printf("Threads: %d\nFile: %s\n\n", threadCount, file);

    // Variables for thread creation
    pthread_t threads[threadCount];
    int status, ret;

    // Create the threads
    for (int i = 0; i < threadCount; i++) {
        status = pthread_create(threads + i, NULL, start, &workers[i]);

        if (status != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for all threads before terminating
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Sum: %f\n", sum);
}