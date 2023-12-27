#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>
#include <ctype.h>
#include <stdint.h>
#include <signal.h>

#define SERVER_PORT                "5000"
#define ERROR_IN_INPUT             9
#define BACKLOG                   4000
#define MAX_CONN_CNT              FD_SETSIZE-5

int debug = 0;

unsigned long long recv_message, send_message;

unsigned long long fact(unsigned long long n);
void error (char *msg);

int main (int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    struct addrinfo hints;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_PASSIVE;    /* for wildcard IP address */

    struct addrinfo *result;
    int s; 
    if ((s = getaddrinfo (NULL, SERVER_PORT, &hints, &result)) != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        exit (EXIT_FAILURE);
    }

    /* Scan through the list of address structures returned by 
       getaddrinfo. Stop when the the socket and bind calls are successful. */

    int listener, optval = 1;
    socklen_t length;
    struct addrinfo *rptr;
    for (rptr = result; rptr != NULL; rptr = rptr -> ai_next) {
        listener = socket (rptr -> ai_family, rptr -> ai_socktype,
                       rptr -> ai_protocol);
        if (listener == -1)
            continue;

        if (setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
            error("setsockopt");

        if (bind (listener, rptr -> ai_addr, rptr -> ai_addrlen) == 0)  // Success
            break;

        if (close (listener) == -1)
            error ("close");
    }

    if (rptr == NULL) {               // Not successful with any address
        //fprintf(stderr, "Not able to bind\n");
        //exit (EXIT_FAILURE);
    }

    freeaddrinfo (result);

    // Mark socket for accepting incoming connections using accept
    if (listen (listener, BACKLOG) == -1){
        //error ("listen");
    }

    socklen_t addrlen;
    fd_set fds, readfds;
    FD_ZERO (&fds);
    FD_ZERO (&readfds);
    FD_SET (listener, &fds);
    int fdmax = listener;
    struct sockaddr_storage client_saddr;
    char str [INET6_ADDRSTRLEN];
    struct sockaddr_in  *ptr=NULL;
    struct sockaddr_in6  *ptr1=NULL;
    int connect_count=0;

    while (1) {
        FD_ZERO(&readfds);
        for (int fd = 0; fd < (fdmax + 1); fd++) {
            if (FD_ISSET (fd, &fds)) {  // fd is ready for reading 
                FD_SET(fd, &readfds);
            }
        }

        // monitor readfds for readiness for reading
        if (select (fdmax + 1, &readfds, NULL, NULL, NULL) == -1){
            //error ("select");
        }    
        
        // Some sockets are ready. Examine readfds
        for (int fd = 0; fd < (fdmax + 1); fd++) {
            if (FD_ISSET (fd, &readfds)) {  // fd is ready for reading 
                if (fd == listener) {  // request for new connection
                    //if (connect_count == MAX_CONN_CNT) continue;
                    addrlen = sizeof (struct sockaddr_storage);
                    int fd_new;
                    if ((fd_new = accept (listener, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
                        error ("accept");

                    if (debug) printf("Server: Connection accepted for socket fd '%d'\n", fd_new);
                    connect_count++;
                    FD_SET (fd_new, &fds); 
                    if (fd_new > fdmax) 
                        fdmax = fd_new;

                    // print IP address of the new client
                    if (client_saddr.ss_family == AF_INET) {
                        ptr = (struct sockaddr_in *) &client_saddr;
                        inet_ntop (AF_INET, &(ptr -> sin_addr), str, sizeof (str));
                    }
                    else if (client_saddr.ss_family == AF_INET6) {
                        ptr1 = (struct sockaddr_in6 *) &client_saddr;
	                    inet_ntop (AF_INET6, &(ptr1 -> sin6_addr), str, sizeof (str));
                    }
                    else
                    {
                        ptr = NULL;
                    }
                }
                else  // data from an existing connection, receive it
                {
                    ssize_t numbytes = read (fd, &recv_message, sizeof (unsigned long long));
                    
                    if (numbytes == -1){
                        //perror ("recv");
                        if (debug) printf ("Socket %d closed by client (-1)\n", fd);
                        if (close (fd) == -1)
                            error ("close");
                        connect_count--;
                        FD_CLR (fd, &fds);
                    }
                    else if (numbytes == 0) {
                        // connection closed by client
                        if (debug) printf ("Socket %d closed by client (0)\n", fd);
                        if (close (fd) == -1)
                            error ("close");
                        connect_count--;
                        FD_CLR (fd, &fds);
                    }
                    else 
                    {
                        // data from client
                        //if (debug) printf("Server: Number recieved by server %llu for socket fd '%d'\n", recv_message, fd);
                        send_message=fact(recv_message);
                        if (write (fd, &send_message, sizeof(unsigned long long)) == -1){
                            //perror ("send");
                        }
                    }
                }
            } // if (fd == ...
        } // for
    } // while (1)

    exit (EXIT_SUCCESS);
} // main

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

void error (char *msg)
{
    perror (msg);
    exit (1);
}
