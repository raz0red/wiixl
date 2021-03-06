--------------------------------------------
WiiXL v0.1 release README
--------------------------------------------

Ported by raz0red
[http://www.twitchasylum.com]

--------------------------------------------
What is WiiXL?
--------------------------------------------

WiiXL is a port of the Atari800 emulator 2.0.3 to the Wii.

The Atari800 emulator is a free and portable Atari 800/XL/XE/5200 emulator, 
originally written by David Firth and now developed by the Atari800 Development 
Team [http://atari800.sourceforge.net/].

--------------------------------------------
Current status
--------------------------------------------

WiiXL is definitely a work in progress. At this point much of the emulator's
extended functionality is only accessible via a USB keyboard. While most of the 
common buttons/keys have been mapped to the Wii/GameCube controllers others 
are unavailable (HELP, PAUSE, etc.). Also, if you attempt to use functionality
such as state loading/saving you will be prompted to enter a file name. Until
a virtual keyboard is implemented, this must be accomplished via USB keyboard.
I will also be adding user-defined key mappings in a subsequent release.

--------------------------------------------
Installation
--------------------------------------------

To install WiiXL, simply extract the zip file that this README was distributed 
with directly to your SD card (retain the hierarchical structure exactly).

WiiXL includes the Atari 800/XL/XE BIOS, OS, and BASIC ROM files. 

However, it does NOT include the Atari 5200 ROM file. If you want play Atari 
5200 games you must obtain the ROM file, "5200.rom", and place it in the WiiXL 
application directory (/apps/wiixl).

Disk and cartridge images should be placed in the disks directory (wiixl/disks).

--------------------------------------------
Controls
--------------------------------------------

    WiiXL menu:
    -----------

        Wiimote:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Exit to HBC  : Home            
            Power off    : Power

         Classic controller:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Exit to HBC  : Home            
                  
         GameCube controller:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Exit to HBC  : Z            
            
    Atari800 emulator menu:
    -----------------------
    
        Wiimote:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Cold start   : +
            Warm start   : -
            Exit to HBC  : Home
            Power off    : Power

         Classic controller:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Cold start   : +
            Warm start   : -
            Exit to HBC  : Home
                  
         GameCube controller:

            Scroll       : Directional pad
            Select       : A
            Back         : B
            Cold start   : R Trigger
            Warm start   : L Trigger
            Exit to HBC  : Z Trigger
            
         USB keyboard:
         
            Refer to Atari800 emulator documentation.             
            
    Atari 800/XL/XE emulation:
    --------------------------

        Wiimote:

            Move       : Directional pad
            Fire       : 2
            [Start]    : +
            [Select]   : -
            [Option]   : A            
            Emu Menu   : Home
            Power off  : Power

         Classic controller:

            Move       : Directional pad
            Fire       : A
            [Start]    : +
            [Select]   : -
            [Option]   : R Trigger            
            Emu Menu   : Home
                  
         GameCube controller:

            Move       : Directional pad
            Fire       : A
            [Start]    : Start
            [Select]   : L Trigger
            [Option]   : R Trigger            
            Emu Menu   : Z Trigger
           
         USB keyboard:
         
            Refer to Atari800 emulator documentation.             
            (Keyboard joystick emulation is not supported by WiiXL at this time)
                
    Atari 5200 emulation:
    ---------------------

        Wiimote:

            Move       : Directional pad
            Fire       : 2
            Start      : +
            *          : -
            #          : A            
            Emu Menu   : Home
            Power off  : Power

         Classic controller:

            Move       : Directional pad
            Fire       : A
            Start      : +
            *          : -
            #          : R Trigger            
            Emu Menu   : Home
                  
         GameCube controller:

            Move       : Directional pad
            Fire       : A
            Start      : Start
            *          : L Trigger
            #          : R Trigger            
            Emu Menu   : Z Trigger            
            
         USB keyboard:
         
            Refer to Atari800 emulator documentation.             
            (Keyboard joystick emulation is not supported by WiiXL at this time)

--------------------------------------------
WiiXL crashes, code dumps, etc.
--------------------------------------------

If you are having issues with WiiXL, please let me know about it via one of the 
following locations:

[http://www.wiibrew.org/wiki/Talk:WiiXL]
[http://www.twitchasylum.com/forum/viewtopic.php?t=511]

--------------------------------------------
Special thanks
--------------------------------------------

* Tantric/eke-eke  : Audio code example
* yohanes          : SDL USB keyboard support
* Team Twiizers    : For enabling homebrew

--------------------------------------------
Change log
--------------------------------------------

02/07/09 (0.1)
--------------
    - Initial release
    - Wiimote/Classic/GameCube controller support
    - USB Keyboard
    - Audio/Video etc.    
    - Save/load state
    - Screenshot support
    - Audio recording support
