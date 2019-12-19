#ifndef REMOTE_H
#define REMOTE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h> /* kapou sthn select xreiazetai */
#include <sys/select.h> /* sockets */
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <arpa/inet.h>
#include <netdb.h> /* gethostbyaddr */
#include <signal.h> /* signal */
#include <unistd.h> /* fork */
#include <fcntl.h> /* files */

#define MAX_MSG 512

#define TRUE 1
#define FALSE 0

void perror_exit ( char * message )
{
    perror ( message ) ;
    exit ( EXIT_FAILURE ) ;
} 

#endif