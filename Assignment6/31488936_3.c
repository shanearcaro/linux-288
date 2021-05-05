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

int main(int argc, char* argv[]) {
    // Abort if proper arguments are given
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    // Set up arguments
    int childCount = atoi(argv[1]);
    int termCount = atoi(argv[2]);

    // Array of process values used to sum an approximation
    long double piSum[childCount];

    // Declare sub process variables;
    pid_t pid, children[childCount];
    int status;

    // Declare IPC variables
    int pfds[2];
    char buffer[30];

    // Open IPC pipe as parent
    pipe(pfds);

    // Create every child process
    for (int i = 0; i < childCount; i++) {
        if ((children[i] = fork()) < 0) {
            printf("Error forking child process\n");
            exit(1);
        }
        else if (children[i] == 0) {
            // Calculate value of pi here
            double x, h, sum;
            h = 1.0 / (double) termCount;

            int j = (termCount / childCount) * i + 1;
            int r = (termCount / childCount) * (i + 1);
            int jTemp = j;

            for (j; j <= r; j++) {
                x = h * ((double) j - 0.5);
                sum += 4.0 / (1.0 + x * x);
            }

            // Write to buffer
            sprintf(buffer, "%f", sum);
            buffer[strlen(buffer)] = 0;
            write(pfds[1], buffer, strlen(buffer) + 1);

            printf("(Process[%d] PID[%d] PPID[%d]) %5d%5d\t%f\n", i, getpid(), getppid(), jTemp, r, sum);

            // Exit the child process
            exit(0);
        }
        else {
            // Parent process reads in data here
            read(pfds[0], buffer, sizeof(buffer));
            piSum[i] = atof(buffer);
        }
    }

    double childTemp = childCount;

    // Parent will wait for every child process to die before continuing
    while (childTemp > 0) {
        pid = wait(&status);
        childTemp--;
    }

    // Actually calculate the approximation for pi
    double pi = 0.0;
    for (int i = 0; i < childCount; i++) {
        pi += (1.0 / (double) termCount) * piSum[i];
    }

    printf("%f\n", pi);
}