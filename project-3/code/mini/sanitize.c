#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void sanitize ( char* line);


void main()
{

    char buf[100] = "ls -i kokoDir ; cut -f1,3 koko  \0";

    sanitize( buf );

    printf("%s\n" , buf);

}




void sanitize ( char* line)
{
    int i = 0;
    int size = strlen( line );
    while( i < size )
    {
        if (line[i] == ';')
        {
            line[i] = '\0';
        }
        i++;
    }

}