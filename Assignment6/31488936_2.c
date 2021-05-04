#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/**
 * Get memory mapped pointer to a file open for reading
 */
float* getFile(char* path, int* length) {
    int fd;
    int fileSize, count;
    struct stat status;
    float* map;

    // Open file and get file size
    fd = open(path, O_RDWR);

    if (fd == -1) {
        printf("Error opening file for reading\n");
        return NULL;
    }

    fstat(fd, &status);
    fileSize = status.st_size;
    *length = (fileSize / sizeof(float));

    // Map the file to virtual memory
    map = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return map;
}

int main(int argc, char* argv[]) {
    // Abort if proper arguments aren't given
    if (argc != 4) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    // Set up arguments 
    int childCount = atoi(argv[1]);
    char* matrixFile = argv[2];
    char* saveFile = argv[3];

    // Get memory mapped file
    int length;
    float* map = getFile(matrixFile, &length);

    // Abort if valid process count isn't given
    if (childCount > length) {
        printf("Process count cannot be greater than matrix size\n");
        return 1;
    }
    else if (childCount == 0) {
        printf("Process count cannot be 0\n");
        return 1;
    }

    // Abort if file can't be mapped
    if (map == NULL) {
        printf("Could not memory map file\n");
        return 1;
    }

    // Create size of row / col (all matrix will be squared)
    int n = (int) sqrt(length);

    // Create regular matrix and identity matrix
    float matrix[n][n];
    float iMatrix[n][n];

    // Matrix is filled with mapped values
    int mapIndex = 0;
    // Create matrix of values
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[j][i] = map[mapIndex];
            mapIndex++;
        }
    }

    // Identity matrix is filled with 1's and 0's at correct position
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            iMatrix[i][j] = (i == j) ? 1 : 0;
        }
    }


    // Open output file for reading using memory mapping 
    int fd, result;
    int fileSize = length * sizeof(float);
    float* writeMap;

    fd = open(saveFile, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        printf("Error opening file for writing\n");
        exit(1);
    }

    result = lseek(fd, fileSize - 1, SEEK_SET);
    if (result == -1) {
        close(fd);
        printf("Error calling lseek() to 'stretch' the file\n");
        exit(1);
    }

    result = write(fd, "", 1);
    if (result == -1) {
        close(fd);
        printf("Error writing lasy byte of the file\n");
        exit(1);
    }

    writeMap = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (writeMap == MAP_FAILED) {
        close(fd);
        printf("Error mmapping the file\n");
        exit(1);
    }

    /**
     * Everything up to now has been done in the parent thread
     * Child processes are going to start being created
     */

    // Declare sub process variables
    pid_t pid, children[childCount];
    int status, valuesRead = 0;

    // Variables used for calculating work each sub process will have to do
    int work = length / childCount;
    int remainderWork = work;
    int size = sqrt(length);

    // Remainder set for last worker if needed
    if (length % childCount != 0)
        remainderWork = work + length - (childCount * work);

    // Create every child process
    for (int i = 0; i < childCount; i++) {
        if ((children[i] = fork()) < 0) {
            printf("Error forking child process\n");
            exit(1);
        }
        else if (children[i] == 0) {
            // Each child process will begin working here based on their creation index, i
            int workAmount = (i != childCount - 1) ? work : remainderWork;
            int workingRows[workAmount];
            int workingCols[workAmount];
            int matrixRow = 0;
            int matrixCol = 0;

            int changeAmount = work * i;

            // Define where the child process should start working
            for (int j = 0; j < changeAmount; j++) {
                if (++matrixRow >= size) {
                    matrixRow = 0;
                    matrixCol++;
                }
            }

            // Create array of values the child process should work on
            for (int j = 0; j < workAmount; j++) {
                workingRows[j] = matrixRow;
                workingCols[j] = matrixCol;

                matrixRow++;
                if (matrixRow >= size) {
                    matrixRow = 0;
                    matrixCol++;
                }
            }

            // Define where to write in memory
            int writePoint = i * work;
            float matrixValue = 0;

            // Insert matrix value based on matrix multiplication and child process's work instructions
            int workValue = 0;
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    for (int r = 0; r < size; r++) {
                        matrixValue += matrix[k][r] * iMatrix[r][j];
                    }

                    if ((k == workingRows[workValue]) && (j == workingCols[workValue])) {
                        // Value is generated
                        printf("(Process[%d]: PID[%d] PPID[%d]) - writing at [%d][%d] value: %f\n", i, getpid(), getppid(), k, j, matrixValue);
                        writeMap[writePoint++] = matrixValue;
                        workValue++;
                    }
                    matrixValue = 0;
                }
            }
            // Exit the child process
            exit(0);
        }
    }

    // Parent will wait for every child process to die before continuing
    while (childCount > 0) {
        pid = wait(&status);
        childCount--;
    }

    // Matrix multiplied values have been written at this point
    // Unmap the memory file and close the file
    if (pid != 0) {
        if (munmap(writeMap, fileSize) == -1) {
            printf("Error un-mapping the file\n");
            exit(1);
        }
        close(fd);
    }
}