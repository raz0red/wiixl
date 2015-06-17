#include <string.h>
#include <ctype.h>
#include "wii_util.h"

void Util_getextension( char *filename, char *ext )
{
    char *ptr = strrchr( filename, '.' );
    int index = 0;
    if( ptr != NULL )
    {
        ++ptr;    
        while( *ptr != '\0' )
        {
            ext[index++] = tolower(*ptr);        
            ++ptr;
        }
    }
    ext[index] = '\0';
}


