#include "remote.h"

void file_read( char * file_name );

int main( int argc , char ** argv )
{

    /***** elenxos arguments *****/
    assert( argc == 5 );

    char * machine_name = (char * ) malloc( sizeof(argv[1]) );
    machine_name = argv[1];
    
    int server_port = atoi(argv[2]);
    int receive_port = atoi(argv[3]);

    assert( server_port > 0 && receive_port > 0 );

    char * file_name = (char * ) malloc( sizeof(argv[4]) );
    file_name = argv[4];
    
    
    /***** make socket *****/
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


    /***** open file *****/
    file_read( file_name );

    
}


// anoigma kai diavasma arxeiwn
void file_read( char * file_name )
{
    FILE *stream;
    char *line[10] = { NULL };
    size_t len[10] = {0};
    size_t message_size;
    ssize_t nread = 0;
    char * message = NULL;
    int i;
    int k = 0;

    // try to open file
    stream = fopen( file_name , "r" );
    if (stream == NULL)
        perror_exit("fopen");

    while ( nread != -1 )
    {
        message_size = 0;

        // diavasma deka grammwn kai enwsh tous se ena paketo
        for( i = 0 ; i < 10 ; i++ )
        {
            // diavasma grammhs
            nread = getline( &line[i] , &len[i] , stream );
            
            //ipologismos mege8ous mhnumatos
            if ( nread != -1 )
                message_size += (size_t) nread;
            else
                *line[i] = '\0'; // exei teleiwsei to arxeio
        }

        if ( message_size == 0)
            break;

        message = realloc( message , message_size + 2);

        message[0] = '\0';
        for( i = 0 ; i < 10 ; i++ )
        {
            // na mhn bazei kenes grammes ??????????????
            strcat( message, line[i] );
        }

        // fix string format
        char s[2] = "\n\0";
        strcat( message, s);

        // send message
        printf("\nmessage %d\n%ssize = %d\n\n" , k , message , (int) message_size);
        fflush(stdout);
        k++;

    }

    free(message);
    for( i = 0 ; i < 10 ; i++ )
        free( line[i] );
}