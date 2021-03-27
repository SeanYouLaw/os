#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define MAX_SEQUENCE 10

typedef struct {
    long fib_sequence[MAX_SEQUENCE]; //保存fibonacci数列
    int sequence_size; //生成序列的大小
}shared_data; //共享内存段

int main(int argc, char* argv[])
{
    pid_t pid;
    char *shmptr;
    //shared_data data;
    int shmid;

    if (argv[1] == NULL) {
        printf("Need a number.\n");
        exit(-1);
    }else if (atoi(argv[1]) <= 0) {
        printf("Need a positive number.\n");
        exit(-1);
    }else if (atoi(argv[1]) > 10) {
        printf("Too big. Change a smaller number.\n");
        exit(-1);
    }
    
    shmid = shmget(IPC_PRIVATE, (sizeof(char) * 30), (SHM_R | SHM_W));
    shmptr = (char *)shmat(shmid, NULL, 0);   
    //printf("shmid: %d\n", shmid);

    pid = fork();
    if (pid == 0) { //子进程
        printf("In child process!\n");
        //shared_data child_data;
        int f0 = 0;
        int f1 = 1;
        int f2;
        
        printf("...Writing to shared memory...\n");
        sprintf(shmptr,"%d%d",f0,f1);   //写入的int转换成了char
        
        for (int i = 2; i < atoi(argv[1]); i++) {
            f2 = f0 + f1;
            f0 = f1;
            f1 = f2;
            sprintf(shmptr + i, "%d", f2);
        }

        shmdt(shmptr);
        printf("child process over.\n");

    }else { //父进程
        wait(NULL);
        printf("In father process!\n");
        for (int j=0; j < atoi(argv[1]); j++) {
                printf("%c ",*(shmptr + j));
        }
        //printf("%s",shmptr);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;    
}
