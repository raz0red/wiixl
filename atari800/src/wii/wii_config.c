/*
WiiHandy/SDL : Port of the Handy/SDL Emulator for the Wii

Copyright (C) 2009
raz0red (www.twitchasylum.com)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#include <stdio.h>
#include <string.h>

#include "wii_main.h"
#include "wii_file_io.h"
#include "wii_config.h"
#include "wii_util.h"
#include "wii_app.h"
#include "wii_atari.h"

#include "../util.h"

/*
 * Read the configuration file
 *
 * return   Was the read success?
 */
BOOL wii_read_config()
{
    char buff[512];

    char config_loc[WII_MAX_PATH];
    wii_get_app_relative( WII_CONFIG, config_loc );

    FILE *fp;
    fp = fopen( config_loc, "r" );
    if (fp == NULL) 
    {	
        // Remount the SD Card
        wii_remount_sd();

        return FALSE;		
    }

    while( fgets( buff, sizeof( buff ), fp ) ) 
    {
        char *ptr;
        Util_chomp( buff );
        ptr = strchr( buff, '=' );
        if( ptr != NULL ) 
        {
            *ptr++ = '\0';
            Util_trim( buff );
            Util_trim( ptr );

            if( strcmp( buff, "SCREEN_SIZE" ) == 0 )
            {
                wii_scale = Util_sscandec( ptr );
            }
            else if ( strcmp( buff, "DEBUG" ) == 0 )
            {
                wii_debug = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "BW" ) == 0 )
            {
                BW = Util_sscandec( ptr );				
            }
            if( strcmp( buff, "WIDTH_MODE" ) == 0 )
            {
                WIDTH_MODE = Util_sscandec( ptr );
            }
            if( strcmp( buff, "VSYNC" ) == 0 )
            {
                wii_vsync = Util_sscandec( ptr );
            }
            if( strcmp( buff, "SYSTEM" ) == 0 )
            {
                wii_tvmode = Util_sscandec( ptr );
            }
        }
    }

    fclose(fp);

    // Remount the SD Card
    wii_remount_sd();

    return TRUE;
}

/*
 * Write the configuration file
 *
 * return   Was the write success?
 */
BOOL wii_write_config()
{
    char config_loc[WII_MAX_PATH];
    wii_get_app_relative( WII_CONFIG, config_loc );

    FILE *fp;
    fp = fopen( config_loc, "w" );
    if( fp == NULL ) 
    {
        // Remount the SD Card
        wii_remount_sd();

        return FALSE;
    }

    fprintf( fp, "SCREEN_SIZE=%d\n", wii_scale );
    fprintf( fp, "DEBUG=%d\n", wii_debug );
    fprintf( fp, "BW=%d\n", BW );
    fprintf( fp, "WIDTH_MODE=%d\n", WIDTH_MODE );
    fprintf( fp, "VSYNC=%d\n", wii_vsync );
    fprintf( fp, "SYSTEM=%d\n", wii_tvmode );

    fclose(fp);

    // Remount the SD Card
    wii_remount_sd();

    return TRUE;
}

