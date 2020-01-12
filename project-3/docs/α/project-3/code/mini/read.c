#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void main() {
    int ret, fd;
    char buf[50];
    char word[500];

    int j = 0;
    int i;

    if ((fd = open("read-test", O_RDONLY)) < 0)
        perror("open() error");
    else 
    {
        while ( ( ret = read( fd, buf , sizeof(buf)-1 ) ) > 0 ) 
        {
            i = 0;

            while (i < ret) // gia olous tous xarakthres mesa ston buffer
            {
                while (  ( (char) buf[i] != '\n' ) && (i < ret) ) // mexri na brei \n h na teleiwsei o buffer
                    word[j++] = buf[i++];


                if ( buf[i] == '\n' ) 
                { // an teleiwse epeidh brike \n, teleiwse h grammh, thn ektupwnei kai paei gia thn epwmenh
                    word[j] = '\0';

                    int word_size = strlen(word);
                    if ( word_size > 0 && word_size <= 100 )
                        printf("<%s>\n", word );

                    j = 0;
                }

                i++;
            }
        }
        close(fd);
    }
}


