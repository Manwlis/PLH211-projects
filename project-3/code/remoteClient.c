#include "remote.h"


int empty_line( char * line );
void create_message( FILE *stream , int socket_id , int receive_port );
void send_message(char * block_entolwn , int block_entolwn_size , int socket_id , int receive_port );


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
    struct sockaddr_in server;
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

    create_message( stream , socket_id , receive_port );

    
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


    printf("%d\n", ari8mos_entolwn); // prepei na stelnetai sto proccess pou pernei apanthseis
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
    printf("\n%s\nsize = %d\n\n" ,  message , message_size );
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