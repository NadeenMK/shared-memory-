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
    shared_memory *sharedMemy = (shared_memory*) shmat(shmid, (void*)0, 0);

   sharedMemy->in = 0;
   sharedMemy->out = 0;
   sharedMemy->full = 0;
   sharedMemy->empty = BUFFER_SIZE;

    srand(time(0));
    while(1) {
        while(sharedMemy->empty == 0);
        int item = rand() % 100;
        sharedMemy->buffer[sharedMemy->in] = item;
        sharedMemy->in = (sharedMemy->in + 1) % BUFFER_SIZE;
        sharedMemy->full++;
        sharedMemy->empty--;
        printf("Producer produced %d\n", item);
        printf("Number of full cells: %d\n", sharedMemy->full);
        printf("Number of empty cells: %d\n", sharedMemy->empty);
        int delay_time = rand() % 3 + 4;
        printf("Producer delay: %d seconds\n", delay_time);
        delay(delay_time);
    }

    shmdt(sharedMemy);
    return 0;
}
