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


void perror_exit ( char * message )
{
    perror ( message ) ;
    exit ( EXIT_FAILURE ) ;
} 

// telos header den epitrepotan ksexwristo arxeio

int empty_line( char * line );
void create_message( FILE *stream , int socket_id , int receive_port );
void send_message(char * block_entolwn , int block_entolwn_size , int socket_id , int receive_port );
void receiver_work( int receive_port );

int main( int argc , char ** argv )
{
    /***** elenxos arguments *****/
    assert( argc == 5 );

    char * machine_name = (char * ) malloc( sizeof(argv[1]) );
    machine_name = argv[1];
    
    int server_port = atoi(argv[2]);
    assert( server_port > 0 && server_port <= 65535 );

    int receive_port = atoi(argv[3]);
    assert( receive_port > 0 && receive_port <= 65535 );

    char * file_name = (char * ) malloc( sizeof(argv[4]) );
    file_name = argv[4];
    



    /***********************/
    /***** make  child *****/
    /***********************/

    pid_t child_pid;

    if ((child_pid = fork()) < 0)
            perror_exit("creating children");

    if ( child_pid == 0 )
    {
        receiver_work(receive_port);
    }


    /***********************/
    /***** make socket *****/
    /***********************/

    int socket_id;
    struct sockaddr_in server;
    struct hostent * rem ;

    /* Create socket */
    if (( socket_id = socket( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit(" socket ");

    /* Find server address */
    if ( ( rem = gethostbyname( machine_name ) ) == NULL ) // obsolete
    {
        herror("gethostbyname"); // obsolete
        exit(1) ;
    }

    // type of socket created
    server.sin_family = AF_INET ; /* Internet domain */
    memcpy( &server.sin_addr , rem->h_addr_list[0] , rem->h_length );  // giati mem copy kai oxi opos ston server? // h_addr allakse se h_addr_list[0]
    server.sin_port = htons( server_port ); /* Server port */

    /* Initiate connection */
    if ( connect( socket_id , ( struct sockaddr *) &server , sizeof( server ) ) < 0)
        perror_exit(" connect ");


    /*********************/
    /***** open file *****/
    /*********************/
    
    FILE *stream;

    // try to open file
    stream = fopen( file_name , "r" );
    if (stream == NULL)
        perror_exit("fopen");


    /************************************/
    /***** create and send messages *****/
    /************************************/

    create_message( stream , socket_id , receive_port );

    close(socket_id);


    // gia to kleisimo. Den ilopoih8hke
    // wait for child to end
    int status = 0;
    while(1){

    }
    waitpid(child_pid , &status, 0);
}


// anoigma kai diavasma arxeiwn
void create_message( FILE *stream , int socket_id , int receive_port )
{
    char *line[10] = { NULL };
    size_t len[10] = {0};
    size_t block_entolwn_size;
    ssize_t nread = 0;
    char * block_entolwn = NULL;
    int i;
    int ari8mos_entolwn = 0;

    while ( nread != -1 )
    {
        block_entolwn_size = 0;

        // diavasma deka grammwn kai enwsh tous se ena paketo
        for( i = 0 ; i < 10 ; i++ )
        {
            // diavasma grammhs. phdaei kenes grammes
            do{
                nread = getline( &line[i] , &len[i] , stream );
            }
            while ( empty_line( line[i])  && nread != -1 );
            //ipologismos mege8ous mhnumatos
            if ( nread != -1 )
            {
                block_entolwn_size += (size_t) nread;
                ari8mos_entolwn++;
            }
            else
            {
                *line[i] = '\0'; // exei teleiwsei to arxeio
            }
        }

        // den exei kati na steilei
        if ( block_entolwn_size == 0 )
            break;

        // prepei na xei dei oles tis grammes pou einai na steilei gia na kserei poso xwro xreiazetai
        block_entolwn = realloc( block_entolwn , block_entolwn_size );

        block_entolwn[0] = '\0';
        for( i = 0 ; i < 10 ; i++ )
            strcat( block_entolwn, line[i] );

        send_message( block_entolwn , block_entolwn_size , socket_id , receive_port );
    }

    free(block_entolwn);
    for( i = 0 ; i < 10 ; i++ )
        free( line[i] );


    printf("ari8mos entolwn: %d\n", ari8mos_entolwn); // prepei na stelnetai sto proccess pou pernei apanthseis
}



void send_message(char * block_entolwn , int block_entolwn_size , int socket_id , int receive_port )
{
    // 5 8eseis gia to port kai 3 gia to format
    int message_size = block_entolwn_size + 8;
    
    // add receive port to final message
    char message[message_size];
    sprintf( message , "%d\n", receive_port );

    // bazoume tis entoles sto teliko munhma
    strcat( message , block_entolwn );

    // fix message format
    char s[2] = "\n\0";
    strcat( message, s);

    // send message
    fflush(stdout);

    write( socket_id , message , message_size );

    sleep(5);
}


int empty_line( char * line )
{
    int line_size = strlen(line);

    for( int i = 0 ; i < line_size ; i++ )
        if ( line[i] != '\0' && line[i] != '\n' && line[i] != ' ' && line[i] != '\t' )
            return 0;
    return 1;
}


void receiver_work( int receive_port )
{
    /*********************/
    /*** Create socket ***/
    /*********************/

    int sockfd; 

    struct sockaddr_in server , client;

    unsigned int serverlen = sizeof(server);
    unsigned int clientlen = sizeof(client);

    /* Create datagram socket */
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        perror_exit("socket creation failed");


    server.sin_family = AF_INET ; /* Internet domain */
    server.sin_addr.s_addr = htonl( INADDR_ANY ) ;
    server.sin_port = htons( receive_port ) ; /* select a port */

    // gia na mporei na ksanaginei dia8esimo to port kateu8eian
    int reuse = 1;
    if ( setsockopt( sockfd , SOL_SOCKET , SO_REUSEADDR , (const char*)&reuse , sizeof(reuse) ) < 0 )
        perror("setsockopt(SO_REUSEADDR) failed");

    /* bind socket */
    if ( bind ( sockfd , (struct sockaddr *) &server , serverlen ) < 0)
        perror_exit ( " bind " ) ;

    /* Discover selected port */
    if ( getsockname ( sockfd , (struct sockaddr *) &server , &serverlen ) < 0)
        perror_exit ( " getsockname " ) ;


    while(1)
    {
        /********************/
        /* Receive  message */
        /********************/
        
        int nread;
        char buffer[10000]; 

        bzero ( buffer , sizeof(buffer) ) ;

        if ( ( nread = recvfrom ( sockfd , buffer , sizeof ( buffer ) , 0 ,  ( struct sockaddr *) &client  , &clientlen ) ) < 0 )
            perror_exit( "recvfrom" );


        /*******************/
        /*** create file ***/
        /*******************/

        // kataskeuh onomatos
        char file_name[100] = {0};
        int counter = 1;

        sprintf( file_name , "output.%d.%d" , receive_port , counter );

        // psaxnei an uparxei arxeio me auto to onoma
        while( access( file_name, F_OK ) != -1 )
        {
            sprintf( file_name , "output.%d.%d" , receive_port ,  counter );
            counter++;
        }

        // dhmiourgia neou arxeiou
        FILE * file_to_save;
        file_to_save = fopen( file_name, "w" );


        /*******************/
        /*** save file *****/
        /*******************/
        
        if(file_to_save == NULL)
            perror_exit("Unable to create file.");

        char * removed_format = NULL;
        removed_format = &(buffer[1]);

        //an einai keno mhn grapsei tipota
        if( nread > 1)
            fprintf(file_to_save , "%s" , removed_format);

        fclose(file_to_save);


        printf("filename = %s    size = %d\n" , file_name , nread - 1 );
    }
}