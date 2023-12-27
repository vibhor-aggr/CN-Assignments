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
#include <sys/wait.h>

#define SERVER_PORT 5000
#define NUM_CON     4000

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

int main(int argc, char *argv[])
{
    int lfd = 0, fd = 0;
    struct sockaddr_in soc_addr; 
    pid_t pid;

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&soc_addr, '0', sizeof(soc_addr));
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    soc_addr.sin_port = htons(SERVER_PORT); 

    bind(lfd, (struct sockaddr*)&soc_addr, sizeof(soc_addr)); 

    listen(lfd, NUM_CON);

    int wstatus;

    while(1){
        
        fd = accept(lfd, (struct sockaddr*)NULL, NULL);
        if(fd<0){
            exit(EXIT_FAILURE);
        }
        
        unsigned long long num=0;
        
        if((pid=fork())==0){
            
            close(lfd);
            while (1) {
                
                int read_status=0;

                    read_status = read(fd, &num, sizeof(unsigned long long));

                    if (read_status == -1)
                    {
                        //perror("Server: Error in reading number\n");
                        close(fd);
                        return EXIT_FAILURE;
                    }
                    else if(read_status == 0){
                        close(fd);
                        return EXIT_SUCCESS;
                    }  
                    //printf("Server: Number recieved by server %llu\n", num);
                    else if(read_status != 0){
                        //printf("Server: Number recieved by server %llu\n", num);
                        unsigned long long ret = fact(num);
                        int write_status = write(fd, &ret, sizeof(unsigned long long));
                        
                        if (write_status == -1) {
                            //perror("Server: Error in number write");
                            close(fd);
                            return EXIT_FAILURE;
                        }
                    }
            }
        }
        else if(pid>0){
            close(fd);
            wait(&wstatus);
        }
    }
    return EXIT_SUCCESS;
}