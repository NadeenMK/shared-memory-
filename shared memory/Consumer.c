#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define BUFFER_SIZE 5

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int full;
    int empty;
} shared_memory;

void delay(int numberINseconds) {
    int milli_seconds = 1000 * numberINseconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds);
}

int main() {
    key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(shared_memory), 0666|IPC_CREAT);
    if (shmid < 0) {
        printf("Unable to access shared memory. Make sure the producer is running.\n");
        return 1;
    }
    shared_memory *sharedMemy = (shared_memory*) shmat(shmid, (void*)0, 0);

    srand(time(0));
    while(1) {
        if(sharedMemy->full == 0) {
            printf("Buffer is empty Consumer is waiting...\n");
            delay(1);
            continue;
        }
        int item = sharedMemy->buffer[sharedMemy->out];
        sharedMemy->out = (sharedMemy->out + 1) % BUFFER_SIZE;
        sharedMemy->full--;
        sharedMemy->empty++;
        printf("Consumer consumed %d\n", item);
        printf("Number of full cells: %d\n", sharedMemy->full);
        printf("Number of empty cells: %d\n", sharedMemy->empty);
        int delay_time = rand() % 6 + 3;
        printf("Consumer delay: %d seconds\n", delay_time);
        delay(delay_time);
    }

    shmdt(sharedMemy);
    return 0;
}

