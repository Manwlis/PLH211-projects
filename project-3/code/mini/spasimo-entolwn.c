#include "/home/manolis/Documents/GitHub/PLH211-projects/project-3/code/remote.h"

int main()
{
    char    str[]= "     ls       -l  ' s  s ' x ";
    char ** res  = NULL;
    char *  p    = strtok (str, " ");
    int n_spaces = 0, i;


    /* split string and append tokens to 'res' */

    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);

        if (res == NULL)
            exit (-1); /* memory allocation failed */

        res[n_spaces-1] = p;

        p = strtok (NULL, " ");
    }

    /* realloc one extra element for the last NULL */

    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;


    /* print the result */

    for (i = 0; i < (n_spaces+1); ++i)
        printf ("res[%d] = %s\n", i, res[i]);

    /* free the memory allocated */

    free (res);
}