#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

pid_t my_fork();
void print_pids(int fd, short unsigned int N, short unsigned int G);
void count_lines(short unsigned int G);
void print_threads(short unsigned int N);
void* print_thread_message(void* i);

int main(int argc, char* argv[])
{
    FILE* op = fopen("out.txt", "w");
    if(!op)
    {
        perror("Could not open file");
        exit(EXIT_FAILURE);
    }
    // -------------- Running the needed functions---------------
    print_pids(fileno(op), atoi(argv[1]), atoi(argv[2]));
    count_lines(atoi(argv[2]));
    print_threads(atoi(argv[1]));
    return 0;
}

pid_t my_fork(){
    pid_t result = fork();
    if ((int) result >= 0)
        return result;
    perror("Failed to fork\n");
    exit(EXIT_FAILURE);                                                                         //exit with error message
}

void print_pids(int fd, short unsigned int N, short unsigned int G){
    int status, g=0;
    pid_t pid = getpid();
    for (int i=0; i<N && g<G+1; i++){
        if(g<G)                                                                                  //if g==G we do not want to create more children.
            pid = my_fork();
        if(pid == 0 && g<G) {                                                                    //if g is the last generation we do not want to increment the generation.
            g++;
            i=-1;                                                                                //So each process will start the loop with i==0 and not i==1.
        }
        if(g!=G)                                                                                 //Wait for children only if you have.
            waitpid(pid,&status,0);                                                //if a process have no children, the function does nothing.
        if (i==N-1)
            dprintf(fd,"My pid is %ld. My generation is %d\n", (long) getpid(), g);          //like fprintf but with file descriptor instead of FILE pointer.
    }
    if(g > 0)
        kill(getpid(),SIGKILL);
}


void count_lines(short unsigned int G)
{
    //----------------------    This function counts the lines and prints to screen how many were printed by each proccess gen------
    int status;
    pid_t pid;
    int g=0;
    char string[100]; /// Create an array of size 100 for the string to print, we will use sprintf
    do{
        pid = my_fork();
        if(pid == 0)
            g++;
    }while(pid ==0 && g<G);
    waitpid(pid,&status,0);                                                         //Wait for children, such that the ancestor prints last
    sprintf(string,"grep \"generation is %d\" out.txt | wc -l",g);
    FILE* op = popen(string, "r");                                                  //Runs the command in string in the shell environment and returns pointer to read form file
    if(op == NULL){
        perror("failded to execute command");
        exit(EXIT_FAILURE);
    }
    int num_of_lines;
    fscanf(op ,"%d", &num_of_lines);                                                //   take the number of lines we scanned
    printf("Number of lines by processes of generation %d is %d\n", g, num_of_lines);
    pclose(op); // cloes the FILE OF command
    if(g>0)
    {
        kill(getpid(),SIGKILL);
    }
}

void* print_thread_message(void* id){
    int* i;
    i = (int*) id;
    printf("Hi. I'm thread number %d\n", *i);
    pthread_exit(NULL);
}

void print_threads(short unsigned int N){
    pthread_t* thread_list = (pthread_t*) malloc(sizeof(pthread_t)*N);
    int* vals = (int*) malloc(sizeof(int)*N);
    void* retval;
    for(int i=0; i<N; i++){
        vals[i] = i;
        pthread_create(&thread_list[i],NULL,print_thread_message,&vals[i]);
        pthread_join(thread_list[i],&retval);
    }
    free(thread_list);
    free(vals);
}

