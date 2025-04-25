// loadbalancer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SHM_SENDER_KEY 1111
#define SHM_R1_KEY     2222
#define SHM_R2_KEY     3333
#define SHM_R3_KEY     4444

char* attach_segment(int key) {
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(1); }
    char* ptr = (char *) shmat(shmid, NULL, 0);
    if (ptr == (char *) -1) { perror("shmat"); exit(1); }
    return ptr;
}

int main() {
    char *sender_shm = attach_segment(SHM_SENDER_KEY);
    char *r1_shm = attach_segment(SHM_R1_KEY);
    char *r2_shm = attach_segment(SHM_R2_KEY);
    char *r3_shm = attach_segment(SHM_R3_KEY);

    char *receivers[] = { r1_shm, r2_shm, r3_shm };
    int current = 0;

    printf("Load balancer started...\n");

    while (1) {
        if (sender_shm[0] == '1') {
            // Wait for target receiver to be ready
            while (receivers[current][0] == '1') {
                current = (current + 1) % 3;
            }

            // Send message to chosen receiver
            receivers[current][0] = '1';
            strncpy(&receivers[current][1], &sender_shm[1], SHM_SIZE - 1);
            printf("Forwarded to receiver %d: %s\n", current + 1, &sender_shm[1]);

            sender_shm[0] = '0'; // Clear sender flag
            current = (current + 1) % 3;
        }

        usleep(10000); // Avoid busy wait
    }

    return 0;
}
