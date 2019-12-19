#include "remote.h"

void read_from_client (int i);
void sanitize ( char* line);

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
    if ( bind( socket_id , (struct sockaddr *) & server , sizeof(server) ) < 0)
        perror_exit("bind");   


    /**** listen socket *****/
    /* Listen for connections */
    if ( listen( socket_id , 5) < 0) // to 5 deixnei posoi mporoun na perimnoun na sunde8oun. Isws 8elei allagh? 
        perror_exit("listen");

    printf( " Listening for connections to port %d\n" , port );


    /***** listen active file descriptors *****/
    
    /* Init set of active sockets */
    FD_ZERO(&active_fd_set);
    FD_SET( socket_id , &active_fd_set );


    while ( TRUE )
    {
        /* Block until input arrive on an active socket */
        read_fd_set = active_fd_set;
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
                    read_from_client(i);

                }
                
            }
        }
    }

}
 

// diavasma socket
void read_from_client (int filedes)
{
    int ret;

    char buf[50];
    char word[500];  // ta noumera paizontai

    int j = 0;
    int i;

    while ( ( ret = read( filedes, buf , sizeof(buf)-1 ) ) > 0 ) 
    {
        i = 0;
        
        while (i < ret) // gia olous tous xarakthres mesa ston buffer
        {
            while (  ( (char) buf[i] != '\n' ) && (i < ret) ) // mexri na brei \n h na teleiwsei o buffer
                word[j++] = buf[i++];

            if ( buf[i] == '\n' ) // an teleiwse epeidh brike \n, teleiwse h grammh, thn ektupwnei kai paei gia thn epomenh
            {
                word[j] = '\0';

                int word_size = strlen(word);
                if ( word_size > 0 && word_size <= 100 ) // elenxos mege8ous
                    printf("<%s>\n", word ); // eisodos sthn domh ----------------- kai pws 8a blepei thn domh h sunarthsh

                j = 0;
            }
            i++;
        }
    }
}


// clean string from comments
void sanitize ( char* line)
{
    int i = 0;
    int flag = 0;
    int size = strlen( line );
    while( i < size && flag == 0)
    {
        if (line[i] == ';')
        {
            line[i] = '\0';
            flag = 1;
        }
        i++;
    }
}