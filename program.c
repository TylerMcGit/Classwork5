#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>  
#include <sys/shm.h>    
#include <unistd.h>      
#include <sys/wait.h>

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);

    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    int *shared_data = (int *) shmat(shmid, NULL, 0);
    if (shared_data == (int *) -1) {
        perror("shmat failed");
        exit(1);
    }

    *shared_data = 100;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        printf("Child: Read %d from shared memory\n", *shared_data);
        *shared_data += 50;
        printf("Child: Updated value to %d\n", *shared_data);
        shmdt(shared_data);
        exit(0);
    } else {
        wait(NULL);
        printf("Parent: Final value in shared memory is %d\n", *shared_data);
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}   
