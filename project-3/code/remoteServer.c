#include "remote.h"


void read_from_socket( int filedes , struct node * lista_entolwn );
void sceduler( int pipe_dia8esimwn , int pipe_entolwn[][2] , struct node * lista_entolwn );
void sanitize ( char * line);
void child_work( int child_id , int pipe_dia8esimothtas );


int main( int argc , char ** argv )
{
    /***** elenxos arguments *****/

    assert( argc == 3 );

    // den kseroume pws prei na einai kai ti elenxo mporei na xreiazetai
    int port = (unsigned int)atoi(argv[1]);
    assert( port > 0 && port <= 65535 );

    int num_children = (int)atoi(argv[2]);
	assert( num_children >= 1 );


    int i;


    /***********************/
    /***** make socket *****/
    /***********************/

    int socket_id;
    struct sockaddr_in server , client;
    socklen_t clientlen;

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

    /* Listen for connections */
    if ( listen( socket_id , 5) < 0) // to 5 deixnei posoi mporoun na perimnoun na sunde8oun. Isws 8elei allagh? 
        perror_exit("listen");

    printf( " Listening for connections to port %d\n" , port );


    /******************************************************/
    /**** create proccesses *******************************/
    /**** make pipes for parent-child synchronization *****/
    /******************************************************/
    
    /* Reap dead children asynchronously. */
    //signal( SIGCHLD , sigchld_handler );  /////////////////////////////-----------------8a to doume otan pame sta proccess


    // se auto to pipe deixnoun ta paidia oti einai dia8esima
    int pipe_dia8esimwn[2];
    if( pipe(pipe_dia8esimwn) == -1 ) 
        perror_exit("pipe call");

    // se auta ta pipe o goneas deinei sta paidia entoles gia ektelesh
    int pipe_entolwn[num_children][2];


    // make children
    for ( i = 0 ; i < num_children ; i++ )
    {
        // dhmiourgia twn pipe pou stelnei o goneas entoles
        if( pipe( pipe_entolwn[num_children] ) == -1 )
            perror_exit("pipe call");
            
        // dhmiourgia paidiwn
        switch ( fork() ) 
        { 
            case -1: /* Error */
                perror_exit("fork"); 

            case 0: /* Child process */
                // den prepei na blepoun to socket
                close( socket_id );

                // ta paidia mono grafoun sto pipe dia8esimothtas
                close(pipe_dia8esimwn[0]);

                // ta paidia mono diavazoun apo to pipe entolwn tous
                close( pipe_entolwn[i][1] );

                // ta paidia den prepei na blepoun ta pipe twn allwn paidiwn
                for ( int j = 0 ; j < i ; j++)
                    close( pipe_entolwn[j][1] );

                // apostolh twn paidiwn sthn douleia tous. Prepei na kseroun pia einai h seira tous
                child_work( i , pipe_dia8esimwn[1] );

            default: 
                // o goneas mono diavazei sto pipe dia8esimothtas
                close( pipe_dia8esimwn[1] );

                // o goneas mono grafei sta pipe entolwn
                close( pipe_entolwn[i][0] );
        }
    }


    /******************************************/
    /***** listen active file descriptors *****/
    /******************************************/

    /* Init set of active sockets */
    fd_set active_fd_set, read_fd_set;

    FD_ZERO(&active_fd_set);

    // bazoume sto set to socket pou kanei sundeseis
    FD_SET( socket_id , &active_fd_set );

    // bazoume sto set to pipe pou deixnei dia8esimothta
    FD_SET( pipe_dia8esimwn[0] , &active_fd_set );
    /* Init empty list gia entoles */
    struct node * lista_entolwn = NULL;

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
                    int copied_socket_id;
                    
                    // dhmiourgia clonou
                    copied_socket_id = accept( socket_id , (struct sockaddr * ) &client , &clientlen ); // gia to size den eimaste sigouroi
                    if ( copied_socket_id < 0 )
                        perror_exit("accept");
                    
                    printf( " Server: connect from host %s, port %d. \n" , inet_ntoa(client.sin_addr ) , (int) ntohs( client.sin_port ));
                    
                    // kanoume to socket non-blocking
                    int flags = fcntl( copied_socket_id , F_GETFL , 0 );
                    if (flags == -1)
                        perror_exit("fcntl");

                    flags = flags | O_NONBLOCK;

                    if ( fcntl( copied_socket_id , F_SETFL , flags ) == -1 )
                        perror_exit("fcntl");

                    // mpenei sto set me ta sockets pou koitaei to select
                    FD_SET ( copied_socket_id , &active_fd_set );
                }
                // yparxei kapio paidi dia8esimo na kanei douleia
                else if ( i == pipe_dia8esimwn[0] )
                {
                    sceduler( pipe_dia8esimwn[0] , pipe_entolwn , lista_entolwn ); 
                }
                
                // antigrafa socket. Pelaths exei steilei paketo
                else
                {
                    /* sundedemeno socket exei kainourgia dedomena */
                    read_from_socket( i , lista_entolwn );
                }
            }
        }
    }
}
 

// diavasma socket
void read_from_socket( int filedes , struct node * lista_entolwn )
{
    /*
// gia na brw thn ip tou pelath
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(filedes, (struct sockaddr *)&addr, &addr_size);
                        
    printf( " Server: connect from host %s, port %d. \n" , inet_ntoa( addr.sin_addr ) , (int) ntohs( addr.sin_port ));
    */





    int ret;

    char buf[50];
    char entolh[500];  // ta noumera paizontai

    int j = 0;
    int i;

    int prwto_read_flag = TRUE;
    int p = 0;

    while ( ( ret = read( filedes, buf , sizeof(buf)-1 ) ) > 0 ) 
    {
        i = 0;
        char port_string[5];

        //pare prwth grammh mhnumatos giati periexei to port gia apanthsh
        if ( prwto_read_flag )
        {
            // mexri na brei \n
            while ( (char) buf[i] != '\n' )
                port_string[p++] = buf[i++];

            //printf(" %s\n", port_string);
            prwto_read_flag = FALSE;
        }
        // gia olous tous xarakthres mesa ston buffer
        while (i < ret)
        {
            while ( ( (char) buf[i] != '\0' ) &&  ( (char) buf[i] != '\n' ) && (i < ret) ) // mexri na brei \n h na teleiwsei o buffer
            {
                /* an exei perasei auto to mege8os sigoura 8a aporif8ei kai gia na  *
                 * apofigoume buffer overflow den kratame tous epomenous xarakthres */
                if( i > 120 )
                    entolh[j++] = buf[i++];
                else
                    i++;
            }

            if ( buf[i] == '\n' ) // an teleiwse epeidh brike \n, teleiwse h grammh, thn ektupwnei kai paei gia thn epomenh
            {
                entolh[j] = '\0';

                int entolh_size = strlen(entolh);
                if ( entolh_size > 0 && entolh_size <= 100 ) // elenxos mege8ous
                {
                    append( &lista_entolwn , port_string , entolh );
                }

                j = 0;
            }
            i++;
        }
    }
}


/* Blepei an iparxei dia8esimh douleia kai paidi gia na thn kanei. Elenxei poios ektelei ti. */
void sceduler( int pipe_dia8esimwn , int pipe_entolwn[][2] , struct node * lista_entolwn )
{
    //printf("%s<%s>\n", port , entolh ); // eisodos sthn domh -----------------
    // des poios einai o prwtos dia8esimos appo pipe dia8esimothtas
    int * dia8esimos = NULL;

    // koitame oti iparxei dia8esimh douleia
    if ( lista_entolwn == NULL )
        return;

    int nread = read( pipe_dia8esimwn , dia8esimos , sizeof(int) );
    if ( nread == -1 ) // la8os
        perror_exit("read se pipe dia8esimwn");
    else if (nread == 0 ) // den uparxei dia8esimos
        return;

    // pernei douleia apo thn lista
    char * port = NULL;
    char * entolh = NULL;
    pop( &lista_entolwn , port , entolh );

    // format gia to pipe
    char * x =  (char * ) malloc( sizeof(port) + 1 + sizeof(entolh) );
    sprintf( x , "%s@%s" , port , entolh);

    // bale entolh sto antoistixo pipe
    if ( ( write( pipe_entolwn[*dia8esimos][1] , x , (sizeof(entolh) + sizeof(port) + 1) ) ) == -1 )
        perror_exit("write se pipe entolwn");
}


/* Clean string from comments. */
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


void child_work( int child_id , int pipe_dia8esimothtas )
{
    // pes sto mpampa eisai dia8esimos
    if ( write( pipe_dia8esimothtas , &child_id , sizeof(int) ) == -1 ) 
        perror_exit("write sto pipe dia8esimothtas");

    // perimene na sou steilei doulia

    // pare douleia


    // kane douleia

    // steile apanthsh ston pelath

    //ksanagine dia8esimos




    exit(0);
}