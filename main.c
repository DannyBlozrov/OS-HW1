#include <stdio.h>
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "wait.h"
#define BUFFER_SIZE 100
pid_t my_fork(void);
void print_pids (int fd, short unsigned int N, short unsigned int G);
int main() {
    print_pids(1,2,2);
    return 0;
}

pid_t my_fork()
{
    pid_t fork_res=fork();
    if(fork_res>=0) //fork succeeds
    {
        return fork_res;
    }
    else //fork failed
    {
        perror("Failed at fork\n");
        exit(1);
    }

}

