#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define SERVER_PORT 5000
#define NUM_CON     8192

int enable_sem=0;
sem_t sem1;
pthread_t thread_id[NUM_CON];
int fd[NUM_CON];

unsigned long long fact(unsigned long long n){
    if(n>20){
        n=20;
    }
    unsigned long long ret=1;
    
    while(n>1){
        ret*=n;
        n--;
    }
    return ret;
}

void* message(void* arg){
    if (enable_sem) sem_wait(&sem1);
    
    int fd=*((int*)arg);
    unsigned long long num;
    while(1){
        int read_status = read(fd, &num, sizeof(unsigned long long));

        if (read_status == -1)
        {
            //perror("Server: Error in reading number");
            //close(fd);
            //if (enable_sem) sem_post(&sem1);
            //return (void*)EXIT_FAILURE;
        }   
        else if(read_status == 0){
            close(fd);
            if (enable_sem) sem_post(&sem1);
            return (void*)EXIT_SUCCESS;
            //pthread_exit(NULL);
        }  
        //printf("Server: Number recieved by server %llu\n", num);
        else if(read_status!=0){
            //printf("Server: Number recieved by server %llu\n", num);
            unsigned long long ret = fact(num);
            int write_status = write(fd, &ret, sizeof(unsigned long long));
            
            if (write_status == -1) {
                //perror("Server: Error in number write");
                close(fd);
                //return EXIT_FAILURE;
                if (enable_sem) sem_post(&sem1);
                return (void*)EXIT_FAILURE;
            }
        }
    } 
    if (enable_sem) sem_post(&sem1);
    return (void*)EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{   
    signal(SIGPIPE, SIG_IGN);

    int lfd = 0;
    struct sockaddr_in soc_addr; 
    pthread_t p;

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&soc_addr, '0', sizeof(soc_addr));
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    soc_addr.sin_port = htons(SERVER_PORT); 

    bind(lfd, (struct sockaddr*)&soc_addr, sizeof(soc_addr)); 

    listen(lfd, NUM_CON);
    
    int j=0;
    if (enable_sem) sem_init(&sem1, 0, 1);
    while(1){
        if(j>=NUM_CON){
            j=0;
        }
        fd[j] = accept(lfd, (struct sockaddr*)NULL, NULL);
        if(fd[j]<0){
            exit(EXIT_FAILURE);
        }
        int retval;
        if((retval=pthread_create(&(thread_id[j]), NULL, message, (void*)&(fd[j])))!=0){
            printf("Failed to create thread %d", j);
        }
        else{
            pthread_join(thread_id[j], NULL);
        }
        j++;
    }
    
    if (enable_sem) sem_destroy(&sem1);
    close(lfd);
    return EXIT_SUCCESS;
}