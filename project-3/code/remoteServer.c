#include "remote.h"


int read_from_socket( int filedes , struct node ** lista_entolwn );
void sceduler( int pipe_dia8esimwn_read , int pipe_entolwn_write[] , struct node ** lista_entolwn );
void child_work( int child_id , int pipe_dia8esimothtas_write , int pipe_entolwn_read );
int spasimo_entolhs( char * entolh , char ** res , char delim);
int legal_command( const char* string );


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
    int pipe_entolwn_pateras[num_children];

    // kataskeuh pipes entolwn
    for ( i = 0 ; i < num_children ; i++ )
    {
        if( pipe( pipe_entolwn[i] ) == -1 )
            perror_exit("creating pipe %d , i");

        pipe_entolwn_pateras[i] = pipe_entolwn[i][1];
    }

    int pid[num_children];

    // dhmiourgia paidiwn, kleisimo twn pipes pou den xreiazontai kai apostolh gia douleia
    for ( i = 0 ; i < num_children ; i++ )
    {
        if ((pid[i] = fork()) < 0)
            perror_exit("creating children");

        if ( pid[i] == 0)
        {
            //kleise ta tou patera
            close( pipe_dia8esimwn[0] );

            for ( int k = 0 ; k < num_children ; k++ )
            {
                // kleise ola ta write
                close ( pipe_entolwn[i][1] );

                //kleise twn allwn paidiwn ta read
                if ( i != k )
                    close( pipe_entolwn[k][0] );
            }
            // douleia
            child_work( i , pipe_dia8esimwn[1] , pipe_entolwn[i][0] );
        }
    }

    // pateras kleinei oti den xreiazetai
    for ( i = 0 ; i < num_children ; i++ )
    {
        close( pipe_entolwn[i][0] );
        close( pipe_dia8esimwn[1] );
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
                    sceduler( pipe_dia8esimwn[0] , pipe_entolwn_pateras , &lista_entolwn );
                }
                // antigrafa socket. Pelaths exei steilei paketo
                else
                {
                    /* sundedemeno socket exei kainourgia dedomena */
                    int ret = read_from_socket( i , &lista_entolwn );

                    // ekleise to socket apo ton client, to bgazoume apo to set kai to kleinoume kai apo edw
                    if ( ret == 0 )
                    {
                       FD_CLR( i , &active_fd_set );
                       close( i );
                    }
                }
            }
        }
    }
}
 

// diavasma socket
int read_from_socket( int filedes , struct node ** lista_entolwn )
{
    int ret;

    char buf[50];
    char entolh[500];  // ta noumera paizontai

    int j = 0;
    int i;

    int prwto_read_flag = TRUE;
    int p = 0;


    // gia na brw thn ip tou pelath
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    char peer_addr[INET_ADDRSTRLEN];

    int res = getpeername(filedes, (struct sockaddr *)&addr, &addr_size);
    if ( res < 0 )
        perror_exit("getpeername");

    inet_ntop(AF_INET, &(addr.sin_addr), peer_addr, INET_ADDRSTRLEN);

    while ( ( ret = read( filedes, buf , sizeof(buf)-1 ) ) > 0 ) 
    {
        i = 0;
        char port_string[PORT_LEN];

        //pare prwth grammh mhnumatos giati periexei to port gia apanthsh
        if ( prwto_read_flag )
        {
            // mexri na brei \n
            while ( (char) buf[i] != '\n' )
                port_string[p++] = buf[i++];
            port_string[p] = '\0';

            prwto_read_flag = FALSE;
        }

        // gia olous tous xarakthres mesa ston buffer
        while (i < ret)
        {
            while ( ( (char) buf[i] != '\0' ) &&  ( (char) buf[i] != '\n' ) && (i < ret) ) // mexri na brei \n h na teleiwsei o buffer
            {
                /* an exei perasei auto to mege8os sigoura 8a aporif8ei kai gia na  *
                 * apofigoume buffer overflow den kratame tous epomenous xarakthres */
                if( j < ENTOLH_LEN )
                    entolh[j++] = buf[i++];

                else
                    i++;
            }

            if ( buf[i] == '\n' ) // an teleiwse epeidh brike \n, teleiwse h grammh, thn ektupwnei kai paei gia thn epomenh
            {
                entolh[j] = '\0';

                int entolh_size = strlen(entolh);
                
                if ( entolh_size > 0 ) // elenxos mege8ous
                {
                    append( lista_entolwn , peer_addr , port_string , entolh );
                }

                j = 0;
            }
            i++;
        }
    }
    return ret;
}


/* Blepei an iparxei dia8esimh douleia kai paidi gia na thn kanei. Elenxei poios ektelei ti. */
void sceduler( int pipe_dia8esimwn_read , int * pipe_entolwn_write , struct node ** lista_entolwn )
{
    struct node * head = *lista_entolwn;
    // des poios einai o prwtos dia8esimos appo pipe dia8esimothtas
    int dia8esimos;
    // koitame oti iparxei dia8esimh douleia
    
    if ( head == NULL ){
        return;
    }
    
    // psaxnei dia8esimo
    int nread = read( pipe_dia8esimwn_read , &dia8esimos , sizeof(int) );
    if ( nread == -1 ) // la8os
        perror_exit("read se pipe dia8esimwn");
    else if (nread == 0 ) // den uparxei dia8esimos
        return;

    // pernei douleia apo thn lista
    char port[PORT_LEN];
    char entolh[ENTOLH_LEN];
    char peer_addr[INET_ADDRSTRLEN];

    pop( lista_entolwn , peer_addr , port , entolh );

    // format gia to pipe
    char pipe_message[PIPE_MESSAGE_LEN];

    sprintf( pipe_message , "%s@%s@%s" , peer_addr , port , entolh);

    // bale entolh sto antoistixo pipe
    if ( ( write( pipe_entolwn_write[ dia8esimos ] , pipe_message , (sizeof(entolh) + sizeof(port) + 1) ) ) == -1 )
        perror_exit("write se pipe entolwn");

    return;
}


void child_work( int child_id , int pipe_dia8esimothtas_write , int pipe_entolwn_read )
{
    // pes sto mpampa eisai dia8esimos
    if ( write( pipe_dia8esimothtas_write , &child_id , sizeof(child_id) ) == -1 ) 
        perror_exit("write sto pipe dia8esimothtas");

    // perimene na sou steilei doulia
    char pipe_message[PIPE_MESSAGE_LEN];
    int nread = read( pipe_entolwn_read , pipe_message , PIPE_MESSAGE_LEN );
    if ( nread == -1 ) // la8os
        perror_exit("read se pipe entolwn");

    //spaei to mhnuma se peer_addr port kai entolh
    char peer_addr[INET_ADDRSTRLEN];
    char port[PORT_LEN];
    char entolh[ENTOLH_LEN];


    char * delim = "@";
    char * token;

    token = strtok(pipe_message, delim);
    strcpy(peer_addr , token);
    
    token = strtok(NULL, delim);
    strcpy(port , token);

    token = strtok(NULL, delim);
    strcpy(entolh , token);

    token = strtok(NULL, delim);//////?????????????????????????????????????????


    // entolh panw apo 100 la8os
    int len = strlen(entolh);


    // sanitize
    char * ptr;

    ptr = strchr( entolh , ';' ); // sbhnoume oti yparxei meta apo ; allazontas ton me ton termatiko xarakthra
    if (ptr != NULL) {
        *ptr = '\0';
    }
    char * sanitized_entolh = entolh;


    // spasimo entolhs bash twn pipes
    char ** res;
    res = malloc(sizeof(char*));

    int ari8mos_upoentolwn = spasimo_entolhs( sanitized_entolh , res , '|');

    // briskei swstes ipoentoles
    char temp[105];
    delim = " ";
    int swstes_upoentoles = 0;

    for  ( int i = 0 ; i < ari8mos_upoentolwn ; i++ )
    {
        strcpy( temp , res[i] );
        token = strtok( temp , delim );
        
        if ( !legal_command( token ) )
            break;

        swstes_upoentoles++;
    }

    // kataskeuh swsths entolhs enonontas tis swstes ipoentoles me pipes
    char swsth_entwlh[105] = {0};
  
    for  ( int i = 0 ; i < swstes_upoentoles ; i++ )
    {
        strcat( swsth_entwlh , res[i]);
        if ( i < swstes_upoentoles - 1)
            strcat( swsth_entwlh , "| ");
    }

    // kataskeuh onomatos
    char fname[100] = {0};

    int counter = 1;

    sprintf( fname , "output.%s.%d%d" , port , child_id , counter );

    // psaxnei monadiko onoma arxeiou
    while( access( fname, F_OK ) != -1 )
    {
        sprintf( fname , "output.%s.%d%d" , port ,  child_id , counter );
        counter++;
    }

    // dhmiourgia arxeiou
    FILE * fPtr;
    fPtr = fopen( fname, "w" );
    if(fPtr == NULL)
        perror_exit("Unable to create file.");

    // dhmiourgia ekteleshmhs entolhs gia system
    char ektelesimh_entolh[250] = {0};

    if( len < 101 ) // an h entolh einai panw apo 100 xarakthres agnoeitai
        strcat( ektelesimh_entolh , swsth_entwlh);
    strcat( ektelesimh_entolh , " > " );
    strcat( ektelesimh_entolh , fname );

    // ektelesh entolhs
    printf("%s\n" , ektelesimh_entolh);
    fflush(stdout);
    system( ektelesimh_entolh );

    //ksanagine dia8esimos
    child_work(child_id , pipe_dia8esimothtas_write , pipe_entolwn_read );

    printf("child %d dead\n" , child_id);
    exit(0);
}


/* spaei thn entolh se upoentoles */
int  spasimo_entolhs( char * entolh , char ** res , char delim)
{
    char * p = strtok (entolh, &delim);
    int n_spaces = 0;

    /* split string and append tokens to 'res' */
    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);

        if (res == NULL)
            exit (-1); /* memory allocation failed */

        res[n_spaces-1] = p;

        p = strtok (NULL, &delim);
    }

    /* realloc one extra element for the last NULL */
    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;

    return n_spaces;
}


/* elenxei an h entolh einai apo autes pou epitrepontai */
int legal_command( const char* string )
{
    const char * strings[5] = { "ls" , "grep" , "cut" , "cat" , "tr" };

    for (int i = 0; i < 5; i++) {
        if (!strcmp(string, strings[i])) {
            return 1;
        }
    }
    return 0;
}