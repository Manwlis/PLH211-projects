#include "/home/manolis/Documents/GitHub/PLH211-projects/project-3/code/remote.h"

int main()
{


    char * line = "1    \n \t  \0";

    int line_size = strlen(line);
    for( int i = 0 ; i < line_size ; i++ )
    {
        if ( line[i] != '\0' && line[i] != '\n' && line[i] != ' ' && line[i] != '\t' )
            printf("0\n");//return 0;
    }

    printf("1\n");//return 1;



}