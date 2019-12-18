#include <stdio.h>
#include <stdlib.h>
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

#define MAX_MSG 512

#define TRUE 1
#define FALSE 0

void perror_exit ( char * message )
{
    perror ( message ) ;
    exit ( EXIT_FAILURE ) ;
}

int read_from_client (int i)
{
    return i;
}

int main( int argc , char ** argv )
{

    /***** elenxos arguments *****/
    assert( argc == 3 );

    // den kseroume pws prei na einai kai ti elenxo mporei na xreiazetai
    unsigned int port = (unsigned int)atoi(argv[1]);

    int num_children = (int)atoi(argv[2]);
	assert( num_children >= 1 );

    /* Reap dead children a s y n c h r o n o u s l y */
    //signal( SIGCHLD , sigchld_handler ); -----------------8a to doume otan pame sta proccess

    fd_set active_fd_set, read_fd_set;
    int i;
    //size_t size;


    /***** make socket *****/
    int socket_id;
    struct sockaddr_in server , client;
    socklen_t clientlen;

    //struct sockaddr * server_ptr =( struct sockaddr *) & server ; ////unused  opou server_prt to dipla tou exoume balei
    //struct sockaddr * client_ptr =( struct sockaddr *) & client ; ////unused
    //struct hostent * rem ; ///////////////////////////////unused

    /* Create socket */
    if (( socket_id = socket( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit(" socket ");

    // type of socket created
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl( INADDR_ANY );
    server.sin_port = htons( port ); /* The given port */

    // gia na mporei na ksanaginei dia8esimo to port kateu8eian
    int reuse = 1;
    if ( setsockopt( socket_id , SOL_SOCKET , SO_REUSEADDR , (const char*)&reuse , sizeof(reuse) ) < 0 )
        perror("setsockopt(SO_REUSEADDR) failed");

    /* Bind socket to address */
    if ( bind( socket_id , ( struct sockaddr *) & server , sizeof ( server )) < 0)
        perror_exit("bind");   


    /**** listen socket *****/
    /* Listen for connections */
    if ( listen( socket_id , 5) < 0) // to 5 deixnei posoi mporoun na perimnoun na sunde8oun. Isws 8elei allagh? 
        perror_exit("listen");

    printf( " Listening for connections to port %d\n" , port );


    /***** listen active file descriptors *****/

    /* Init set of active sockets */
    while ( TRUE )
    {
        /* Block until input arrive on an active socket */
        if ( select( FD_SETSIZE , &read_fd_set , NULL , NULL , NULL ) < 0 )
            perror_exit("select");

        /* service all the sockets with input pending. */
        for ( i = 0 ; i < FD_SETSIZE ; i++ ) // exei ++i ston original codika!!!!
        {
            if ( FD_ISSET( i , &read_fd_set ) )
            {
                // original socket - kainourgia sundesh
                if ( i == socket_id )
                {
                    int copied_socket_id;  // to metefera apo panw gia na einai san ton kwdika tou select
                    //size = sizeof(client);
                    // dhmiourgia clonou
                    copied_socket_id = accept( socket_id , (struct sockaddr * ) &client , &clientlen ); // gia to size den eimaste sigouroi

                    if ( copied_socket_id < 0 )
                        perror_exit("accept");
                    
                    printf ( " Server: connect from host %s, port %hd. \n" , inet_ntoa(client.sin_addr ) , ntohs( client.sin_port ));
                    
                    // mpenei sto set me ta sockets pou koitaei to select
                    FD_SET ( copied_socket_id , &active_fd_set );
                }
                else
                {
                    /* sundedemeno socket exei kainourgia dedomena */
                    int flag = read_from_client(i);
                    // h read from client mas leei oti teleiwse h douleia tou
                    if( flag <= 0)
                    {
                        close(i);
                        FD_CLR ( i , &active_fd_set );
                    }
                }
                
            }
        }
    }

}
 
