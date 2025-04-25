// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <shm_key>\n", argv[0]);
        exit(1);
    }

    int shm_key = atoi(argv[1]);
    int shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(1); }

    char *shm_ptr = (char *) shmat(shmid, NULL, 0);
    if (shm_ptr == (char *) -1) { perror("shmat"); exit(1); }

    printf("Receiver started with key %d...\n", shm_key);

    while (1) {
        if (shm_ptr[0] == '1') {
            printf("Received on key %d: %s\n", shm_key, &shm_ptr[1]);
            shm_ptr[0] = '0';
        }
        usleep(10000);
    }

    return 0;
}
