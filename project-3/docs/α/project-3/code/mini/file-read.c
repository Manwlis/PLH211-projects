
#include "/home/manolis/Documents/GitHub/PLH211-projects/project-3/code/remote.h"

int main()
{

    FILE *stream;
    char *line[10] = { NULL };
    size_t len[10] = {0};
    size_t message_size;
    ssize_t nread = 0;
    char * message = NULL;
    int i;
    int k = 0;

    stream = fopen("file-read-test", "r");
    if (stream == NULL) {
        perror_exit("fopen");
    }
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

}