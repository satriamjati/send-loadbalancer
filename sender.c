// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SHM_KEY 1111

int main() {
    int shmid;
    char *shm_ptr;

    // Create shared memory
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach to shared memory
    shm_ptr = (char *) shmat(shmid, NULL, 0);
    if (shm_ptr == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        char input[SHM_SIZE - 1];
        printf("Enter message to send: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline

        // Wait until receiver clears the flag
        while (shm_ptr[0] == '1') {
            usleep(10000); // 10ms sleep
        }

        // Write message and set flag
        shm_ptr[0] = '1';
        strncpy(&shm_ptr[1], input, SHM_SIZE - 1);
    }

    // Detach
    shmdt(shm_ptr);
    return 0;
}
