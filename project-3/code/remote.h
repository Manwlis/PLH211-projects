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
#define PORT_LEN 6
#define ENTOLH_LEN 105
#define PIPE_MESSAGE_LEN (INET_ADDRSTRLEN + 1 + PORT_LEN + 1 + ENTOLH_LEN + 1)

#define TRUE 1
#define FALSE 0


struct node
{
    char peer_addr[INET_ADDRSTRLEN];
    char port[6];
    char entolh[105];
    struct node * next;
};


void append( struct node ** list, char * address , char * port , char * entolh ) 
{ 
    /* 1. allocate node */
    struct node * new_node = (struct node*) malloc(sizeof(struct node));

    struct node * last = *list;  /* used in step 5*/

    /* 2. put in the data  */
    int i;

    for( i = 0 ; i < INET_ADDRSTRLEN ; i++ )
        new_node->peer_addr[i] = address[i];

    for( i = 0 ; i < PORT_LEN ; i++ )
        new_node->port[i] = port[i];
    new_node->port[i] = '\0';

    for( i = 0 ; i < ENTOLH_LEN ; i++ )
        new_node->entolh[i] = entolh[i];

    /* 3. This new node is going to be the last node, so make next of it as NULL*/
    new_node->next = NULL;
    
    /* 4. If the Linked List is empty, then make the new node as head */
    if (*list == NULL) 
    { 
       *list = new_node; 
       return; 
    } 
  
    /* 5. Else traverse till the last node */
    while (last->next != NULL) 
        last = last->next; 
  
    /* 6. Change the next of last node */
    last->next = new_node;

    return; 
}


/* returns 0 on empty list, else 1 */
int pop( struct node **list , char * address , char * port , char * entolh ) 
{ 
    // If linked list is empty 
    if (*list == NULL) 
        return 0; 

    // Store head node
    struct node* head = *list; 

    /* put out the data  */
    for( int i = 0 ; i < INET_ADDRSTRLEN ; i++ )
       address[i] = head->peer_addr[i];

    for( int i = 0 ; i < PORT_LEN ; i++ )
       port[i] = head->port[i];

    for( int i = 0 ; i < ENTOLH_LEN + 1 ; i++ )
       entolh[i] = head->entolh[i];

    /* delete head  */
    *list = head->next; // Change head

    free(head);

    return 1;
} 


void perror_exit ( char * message )
{
    perror ( message ) ;
    exit ( EXIT_FAILURE ) ;
} 

#endif