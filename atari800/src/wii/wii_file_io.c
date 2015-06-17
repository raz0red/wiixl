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

#include <fat.h>
#include <gccore.h>
#include <sdcard/wiisd_io.h>

#include "wii_main.h"
#include "wii_file_io.h"


// Is the SD card mounted?
static BOOL sd_mounted = FALSE;

/*
 * Unmounts the SD card
 */
void wii_unmount_sd()
{
    if( sd_mounted )
    {
        fatUnmount( "sd:/" );
        __io_wiisd.shutdown();

        sd_mounted = FALSE;
    }
}

/*
 * Mounts the SD card
 */
BOOL wii_mount_sd()
{
    if( !sd_mounted )
    {
        if( !__io_wiisd.startup() )
        {
            return FALSE;
        }
        else if( !fatMountSimple( "sd", &__io_wiisd ) )
        {
            return FALSE;
        }
        else
        {
            //fatSetReadAhead( "sd:/", 6, 64 );
        }

        chdir( WII_FILES_DIR );

        sd_mounted = TRUE;
    }

    return TRUE;
}

/*
 * Remounts the SD card 
 */
void wii_remount_sd()
{ 
    // For some reason people are experiencing a lot of strange bugs on newer
    // firmware versions (3.3 and 3.4). These problems didn't seem to occur on
    // 3.2. The issue seems related to file caching. For example, a saved
    // snapshot would have an affect on the next ROM that was loaded, etc. To
    // workaround this, I remount the SD Card after each significant file 
    // operation. This *seems* to workaround the problem. This is definitely 
    // a bit heavy handed and is something I want to resolve in subsequent 
    // release.
    wii_unmount_sd();
    wii_mount_sd();
}
