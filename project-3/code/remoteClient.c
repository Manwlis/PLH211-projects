#include "remote.h"


int empty_line( char * line );
void create_message( FILE *stream , int socket_id);
void send_message(char * message , int message_size , int socket_id );


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
    /***** make socket *****/
    /***********************/

    int socket_id;
    struct sockaddr_in server , client;
    socklen_t clientlen;
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
    if ( connect( socket_id , ( struct sockaddr *) & server , sizeof( server ) ) < 0)
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

    create_message( stream , socket_id );

    
}


// anoigma kai diavasma arxeiwn
void create_message( FILE *stream , int socket_id )
{
    char *line[10] = { NULL };
    size_t len[10] = {0};
    size_t message_size;
    ssize_t nread = 0;
    char * message = NULL;
    int i;

    while ( nread != -1 )
    {
        message_size = 0;

        // diavasma deka grammwn kai enwsh tous se ena paketo
        for( i = 0 ; i < 10 ; i++ )
        {
            // diavasma grammhs. phdaei kenes grammes
            do{
                nread = getline( &line[i] , &len[i] , stream );
            }
            while ( empty_line( line[i] ) );                    //line[i][0] == '\n' || line[i][0] == '\0'

            //ipologismos mege8ous mhnumatos
            if ( nread != -1 )
                message_size += (size_t) nread;
            else
                *line[i] = '\0'; // exei teleiwsei to arxeio
        }

        // den exei kati na steilei
        if ( message_size == 0 )
            break;

        // prepei na xei dei oles tis grammes pou einai na steilei gia na kserei poso xwro xreiazetai
        message_size = message_size + 2;
        message = realloc( message , message_size );

        message[0] = '\0';
        for( i = 0 ; i < 10 ; i++ )
            strcat( message, line[i] );

        send_message( message , message_size , socket_id );
    }

    free(message);
    for( i = 0 ; i < 10 ; i++ )
        free( line[i] );
}



void send_message(char * message , int message_size , int socket_id )
{
        // fix message format
        char s[2] = "\n\0";
        strcat( message, s);

        // send message
        printf("\n%s\nsize = %d\n\n" ,  message , (int) message_size);
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