/*
* atari_sdl.c - SDL library specific port code
*
* Copyright (c) 2001-2002 Jacek Poplawski
* Copyright (C) 2001-2005 Atari800 development team (see DOC/CREDITS)
*
* This file is part of the Atari800 emulator project which emulates
* the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
*
* Atari800 is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* Atari800 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Atari800; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
Thanks to David Olofson for scaling tips!

TODO:
- implement all Atari800 parameters
- use mouse and real joystick
- turn off fullscreen when error happen
*/

#include "config.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Atari800 includes */
#include "input.h"
#include "colours.h"
#include "monitor.h"
#include "platform.h"
#include "ui.h"
#include "screen.h"
#include "pokeysnd.h"
#include "gtia.h"
#include "antic.h"
#include "devices.h"
#include "cpu.h"
#include "memory.h"
#include "pia.h"
#include "log.h"
#include "util.h"
#include "atari_ntsc.h"

#include <wiiuse/wpad.h>
#include <gccore.h>
#include <fat.h>
#include <wii_audio.h>

#include "wii_main.h"
#include "wii_atari.h"

static int sound_enabled = TRUE;
static int sound_flags = 0;

static int SDL_ATARI_BPP = 8;
static int SWAP_JOYSTICKS = 0;

int BW = 0;
int WIDTH_MODE = 0;
int wii_exit_emulation = 0;
int wii_vsync = VSYNC_DISABLED;
int wii_tvmode = TV_NTSC;

/* joystick emulation
keys are loaded from config file
Here the defaults if there is no keymap in the config file... */

/* a runtime switch for the kbd_joy_X_enabled vars is in the UI */
int kbd_joy_0_enabled = FALSE;	/* enabled by default, doesn't hurt */
int kbd_joy_1_enabled = FALSE;	/* disabled, would steal normal keys */

#if 0
int KBD_TRIG_0 = SDLK_LCTRL;
int KBD_STICK_0_LEFT = SDLK_KP4;
int KBD_STICK_0_RIGHT = SDLK_KP6;
int KBD_STICK_0_DOWN = SDLK_KP2;
int KBD_STICK_0_UP = SDLK_KP8;
int KBD_STICK_0_LEFTUP = SDLK_KP7;
int KBD_STICK_0_RIGHTUP = SDLK_KP9;
int KBD_STICK_0_LEFTDOWN = SDLK_KP1;
int KBD_STICK_0_RIGHTDOWN = SDLK_KP3;

int KBD_TRIG_1 = SDLK_TAB;
int KBD_STICK_1_LEFT = SDLK_a;
int KBD_STICK_1_RIGHT = SDLK_d;
int KBD_STICK_1_DOWN = SDLK_x;
int KBD_STICK_1_UP = SDLK_w;
int KBD_STICK_1_LEFTUP = SDLK_q;
int KBD_STICK_1_RIGHTUP = SDLK_e;
int KBD_STICK_1_LEFTDOWN = SDLK_z;
int KBD_STICK_1_RIGHTDOWN = SDLK_c;
#endif

extern int alt_function;

static int dsprate = 48000;
static SDL_AudioCVT audio_convert;

/* video */
SDL_Surface *MainScreen = NULL;
SDL_Color colors[256];			/* palette */
Uint32 Palette32[256];			/* 32-bit palette */

/* keyboard */
Uint8 *kbhits = NULL;

/* For better handling of the Atari_Configure-recognition...
Takes a keySym as integer-string and fills the value
into the retval referentially.
Authors: B.Schreiber, A.Martinez
fixed and cleaned up by joy */
int SDLKeyBind(int *retval, char *sdlKeySymIntStr)
{
    int ksym;

    if (retval == NULL || sdlKeySymIntStr == NULL) {
        return FALSE;
    }

    /* make an int out of the keySymIntStr... */
    ksym = Util_sscandec(sdlKeySymIntStr);

    if (ksym > SDLK_FIRST && ksym < SDLK_LAST) {
        *retval = ksym;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/* For getting sdl key map out of the config...
Authors: B.Schreiber, A.Martinez
cleaned up by joy */
int Atari_Configure(char *option, char *parameters)
{
#if 0
    if (strcmp(option, "SDL_JOY_0_ENABLED") == 0) {
        kbd_joy_0_enabled = (parameters != NULL && parameters[0] != '0');
        return TRUE;
    }
    else if (strcmp(option, "SDL_JOY_1_ENABLED") == 0) {
        kbd_joy_1_enabled = (parameters != NULL && parameters[0] != '0');
        return TRUE;
    }
    else if (strcmp(option, "SDL_JOY_0_LEFT") == 0)
        return SDLKeyBind(&KBD_STICK_0_LEFT, parameters);
    else if (strcmp(option, "SDL_JOY_0_RIGHT") == 0)
        return SDLKeyBind(&KBD_STICK_0_RIGHT, parameters);
    else if (strcmp(option, "SDL_JOY_0_DOWN") == 0)
        return SDLKeyBind(&KBD_STICK_0_DOWN, parameters);
    else if (strcmp(option, "SDL_JOY_0_UP") == 0)
        return SDLKeyBind(&KBD_STICK_0_UP, parameters);
    else if (strcmp(option, "SDL_JOY_0_LEFTUP") == 0)
        return SDLKeyBind(&KBD_STICK_0_LEFTUP, parameters);
    else if (strcmp(option, "SDL_JOY_0_RIGHTUP") == 0)
        return SDLKeyBind(&KBD_STICK_0_RIGHTUP, parameters);
    else if (strcmp(option, "SDL_JOY_0_LEFTDOWN") == 0)
        return SDLKeyBind(&KBD_STICK_0_LEFTDOWN, parameters);
    else if (strcmp(option, "SDL_JOY_0_RIGHTDOWN") == 0)
        return SDLKeyBind(&KBD_STICK_0_RIGHTDOWN, parameters);
    else if (strcmp(option, "SDL_TRIG_0") == 0) // obsolete
        return SDLKeyBind(&KBD_TRIG_0, parameters);
    else if (strcmp(option, "SDL_JOY_0_TRIGGER") == 0)
        return SDLKeyBind(&KBD_TRIG_0, parameters);
    else if (strcmp(option, "SDL_JOY_1_LEFT") == 0)
        return SDLKeyBind(&KBD_STICK_1_LEFT, parameters);
    else if (strcmp(option, "SDL_JOY_1_RIGHT") == 0)
        return SDLKeyBind(&KBD_STICK_1_RIGHT, parameters);
    else if (strcmp(option, "SDL_JOY_1_DOWN") == 0)
        return SDLKeyBind(&KBD_STICK_1_DOWN, parameters);
    else if (strcmp(option, "SDL_JOY_1_UP") == 0)
        return SDLKeyBind(&KBD_STICK_1_UP, parameters);
    else if (strcmp(option, "SDL_JOY_1_LEFTUP") == 0)
        return SDLKeyBind(&KBD_STICK_1_LEFTUP, parameters);
    else if (strcmp(option, "SDL_JOY_1_RIGHTUP") == 0)
        return SDLKeyBind(&KBD_STICK_1_RIGHTUP, parameters);
    else if (strcmp(option, "SDL_JOY_1_LEFTDOWN") == 0)
        return SDLKeyBind(&KBD_STICK_1_LEFTDOWN, parameters);
    else if (strcmp(option, "SDL_JOY_1_RIGHTDOWN") == 0)
        return SDLKeyBind(&KBD_STICK_1_RIGHTDOWN, parameters);
    else if (strcmp(option, "SDL_TRIG_1") == 0) // obsolete
        return SDLKeyBind(&KBD_TRIG_1, parameters);
    else if (strcmp(option, "SDL_JOY_1_TRIGGER") == 0)
        return SDLKeyBind(&KBD_TRIG_1, parameters);
    else
        return FALSE;
#endif
    return FALSE;
}

/* Save the keybindings and the keybindapp options to the config file...
Authors: B.Schreiber, A.Martinez
cleaned up by joy */
void Atari_ConfigSave(FILE *fp)
{
#if 0
    fprintf(fp, "SDL_JOY_0_ENABLED=%d\n", kbd_joy_0_enabled);
    fprintf(fp, "SDL_JOY_0_LEFT=%d\n", KBD_STICK_0_LEFT);
    fprintf(fp, "SDL_JOY_0_RIGHT=%d\n", KBD_STICK_0_RIGHT);
    fprintf(fp, "SDL_JOY_0_UP=%d\n", KBD_STICK_0_UP);
    fprintf(fp, "SDL_JOY_0_DOWN=%d\n", KBD_STICK_0_DOWN);
    fprintf(fp, "SDL_JOY_0_LEFTUP=%d\n", KBD_STICK_0_LEFTUP);
    fprintf(fp, "SDL_JOY_0_RIGHTUP=%d\n", KBD_STICK_0_RIGHTUP);
    fprintf(fp, "SDL_JOY_0_LEFTDOWN=%d\n", KBD_STICK_0_LEFTDOWN);
    fprintf(fp, "SDL_JOY_0_RIGHTDOWN=%d\n", KBD_STICK_0_RIGHTDOWN);
    fprintf(fp, "SDL_JOY_0_TRIGGER=%d\n", KBD_TRIG_0);

    fprintf(fp, "SDL_JOY_1_ENABLED=%d\n", kbd_joy_1_enabled);
    fprintf(fp, "SDL_JOY_1_LEFT=%d\n", KBD_STICK_1_LEFT);
    fprintf(fp, "SDL_JOY_1_RIGHT=%d\n", KBD_STICK_1_RIGHT);
    fprintf(fp, "SDL_JOY_1_UP=%d\n", KBD_STICK_1_UP);
    fprintf(fp, "SDL_JOY_1_DOWN=%d\n", KBD_STICK_1_DOWN);
    fprintf(fp, "SDL_JOY_1_LEFTUP=%d\n", KBD_STICK_1_LEFTUP);
    fprintf(fp, "SDL_JOY_1_RIGHTUP=%d\n", KBD_STICK_1_RIGHTUP);
    fprintf(fp, "SDL_JOY_1_LEFTDOWN=%d\n", KBD_STICK_1_LEFTDOWN);
    fprintf(fp, "SDL_JOY_1_RIGHTDOWN=%d\n", KBD_STICK_1_RIGHTDOWN);
    fprintf(fp, "SDL_JOY_1_TRIGGER=%d\n", KBD_TRIG_1);
#endif
}

/* used in UI to show how the keyboard joystick is mapped */
char *joy_0_description(char *buffer, int maxsize)
{
#if 0
    snprintf(buffer, maxsize, " (L=%s R=%s U=%s D=%s B=%s)",
        SDL_GetKeyName(KBD_STICK_0_LEFT),
        SDL_GetKeyName(KBD_STICK_0_RIGHT),
        SDL_GetKeyName(KBD_STICK_0_UP),
        SDL_GetKeyName(KBD_STICK_0_DOWN),
        SDL_GetKeyName(KBD_TRIG_0)
        );
#endif 
    snprintf( buffer, maxsize, "(not implemented)" );

    return buffer;
}

char *joy_1_description(char *buffer, int maxsize)
{
#if 0
    snprintf(buffer, maxsize, " (L=%s R=%s U=%s D=%s B=%s)",
        SDL_GetKeyName(KBD_STICK_1_LEFT),
        SDL_GetKeyName(KBD_STICK_1_RIGHT),
        SDL_GetKeyName(KBD_STICK_1_UP),
        SDL_GetKeyName(KBD_STICK_1_DOWN),
        SDL_GetKeyName(KBD_TRIG_1)
        );
#endif
    snprintf( buffer, maxsize, "(not implemented)" );

    return buffer;
}

static void SetPalette(void)
{
    SDL_SetPalette(MainScreen, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
}

void CalcPalette(void)
{
    int i, rgb;
    float y;
    Uint32 c;
    if (BW == 0)
        for (i = 0; i < 256; i++) {
            rgb = colortable[i];
            colors[i].r = (rgb & 0x00ff0000) >> 16;
            colors[i].g = (rgb & 0x0000ff00) >> 8;
            colors[i].b = (rgb & 0x000000ff) >> 0;
        }
    else
        for (i = 0; i < 256; i++) {
            rgb = colortable[i];
            y = 0.299 * ((rgb & 0x00ff0000) >> 16) +
                0.587 * ((rgb & 0x0000ff00) >> 8) +
                0.114 * ((rgb & 0x000000ff) >> 0);
            colors[i].r = y;
            colors[i].g = y;
            colors[i].b = y;
        }

        for (i = 0; i < 256; i++) {
            c =
                SDL_MapRGB(MainScreen->format, colors[i].r, colors[i].g,
                colors[i].b);
        }
}

void ModeInfo(void)
{
    char bwflag, width, joyflag;
    if (BW)
        bwflag = '*';
    else
        bwflag = ' ';
    switch (WIDTH_MODE) {
    case FULL_WIDTH_MODE:
        width = 'f';
        break;
    case DEFAULT_WIDTH_MODE:
        width = 'd';
        break;
    case SHORT_WIDTH_MODE:
        width = 's';
        break;
    default:
        width = '?';
        break;
    }
    if (SWAP_JOYSTICKS)
        joyflag = '*';
    else
        joyflag = ' ';
    Aprint("Video Mode: %dx%dx%d", MainScreen->w, MainScreen->h,
        MainScreen->format->BitsPerPixel);
    Aprint("[%c] BW  [%c] WIDTH MODE  [%c] JOYSTICKS SWAPPED",
        bwflag, width, joyflag);
}

void SetVideoMode(int w, int h, int bpp)
{
    MainScreen = SDL_SetVideoMode(w, h, bpp, SDL_FULLSCREEN );

    if (MainScreen == NULL) {
        Aprint("Setting Video Mode: %dx%dx%d FAILED", w, h, bpp);
        Aflushlog();
        exit(-1);
    }
}

void SetNewVideoMode(int w, int h, int bpp)
{
    float ww, hh;

    if( wii_scale == -1 )
    {
        w = 640;
        h = 480;
    }
    else
    {
        w = ATARI_WIDTH;
        h = ATARI_HEIGHT;

        if( wii_scale > 0 )
        {
            float scale = 1.0 + (wii_scale * 0.1);
            w *= scale;
            h *= scale;
        }
    }

    if ((h < ATARI_HEIGHT) || (w < ATARI_WIDTH)) {
        h = ATARI_HEIGHT;
        w = ATARI_WIDTH;
    }

    /* aspect ratio, floats needed */
    ww = w;
    hh = h;
    switch (WIDTH_MODE) {
    case SHORT_WIDTH_MODE:
        if (ww * 0.75 < hh)
            hh = ww * 0.75;
        else
            ww = hh / 0.75;
        break;
    case DEFAULT_WIDTH_MODE:
        if (ww / 1.4 < hh)
            hh = ww / 1.4;
        else
            ww = hh * 1.4;
        break;
    case FULL_WIDTH_MODE:
        if (ww / 1.6 < hh)
            hh = ww / 1.6;
        else
            ww = hh * 1.6;
        break;
    }
    w = ww;
    h = hh;
    w = w / 8;
    w = w * 8;
    h = h / 8;
    h = h * 8;

#if 0
    fprintf( stderr, "\n\nwidth: %d, height: %d", w, h );
    wii_pause();
#endif

    SetVideoMode(w, h, bpp);

    SDL_ATARI_BPP = MainScreen->format->BitsPerPixel;
    if (bpp == 0) {
        Aprint("detected %dbpp", SDL_ATARI_BPP);
        if ((SDL_ATARI_BPP != 8) && (SDL_ATARI_BPP != 16)
            && (SDL_ATARI_BPP != 32)) {
                Aprint("it's unsupported, so setting 8bit mode (slow conversion)");
                SetVideoMode(w, h, 8);
        }
    }

    SetPalette();

    SDL_ShowCursor(SDL_DISABLE);	/* hide mouse cursor */

    ModeInfo();
}

void SwitchFullscreen(void)
{
    SetNewVideoMode(MainScreen->w, MainScreen->h,
        MainScreen->format->BitsPerPixel);
    Atari_DisplayScreen();
}

void SwitchWidth(void)
{
    WIDTH_MODE++;
    if (WIDTH_MODE > FULL_WIDTH_MODE)
        WIDTH_MODE = SHORT_WIDTH_MODE;
    SetNewVideoMode(MainScreen->w, MainScreen->h,
        MainScreen->format->BitsPerPixel);
    Atari_DisplayScreen();
}

void SwitchBW(void)
{
    BW = 1 - BW;
    CalcPalette();
    SetPalette();
    ModeInfo();
}

void SwapJoysticks(void)
{
    SWAP_JOYSTICKS = 1 - SWAP_JOYSTICKS;
    ModeInfo();
}

void Sound_Pause(void)
{
    if( sound_enabled ) 
    {
        StopAudio();
    }
}

void Sound_Continue(void)
{
    if( sound_enabled  && !wii_exit_emulation ) 
    {
        ResetAudio();
    }
}

void Sound_Update( void )
{
    if( !wii_exit_emulation )
    {
        static u8 buffer[4096];
        unsigned int nsamples = (tv_mode == TV_NTSC) ? (48000 / 60) : (48000 / 50);
        Pokey_process( buffer, nsamples );

        audio_convert.buf = buffer;
        audio_convert.len = nsamples;
        SDL_ConvertAudio( &audio_convert );

        PlaySound( (u32*)buffer, ( audio_convert.len_cvt / 4 ) );        
    }
}

// Button states
u32 down;
u32 held;
u32 gcDown;
u32 gcHeld;
u32 down2;
u32 held2;
u32 gcDown2;
u32 gcHeld2;

int Atari_Keyboard(void)
{
    key_consol = CONSOL_NONE;

    WPAD_ScanPads();
    PAD_ScanPads();

    down = WPAD_ButtonsDown( 0 );
    held = WPAD_ButtonsHeld( 0 );
    gcDown = PAD_ButtonsDown( 0 );
    gcHeld = PAD_ButtonsHeld( 0 );

    down2 = WPAD_ButtonsDown( 1 );
    held2 = WPAD_ButtonsHeld( 1 );
    gcDown2 = PAD_ButtonsDown( 1 );
    gcHeld2 = PAD_ButtonsHeld( 1 );

    // Power or reset...
    if( wii_hw_button )
    {
        return AKEY_EXIT;
    }

    // While the UI is being displayed
    if( ui_is_active ) 
    {
        // U-D-L-R
        if( held & WII_BUTTON_LEFT || gcHeld & GC_BUTTON_LEFT )
            return AKEY_LEFT;
        if( held & WII_BUTTON_RIGHT || gcHeld & GC_BUTTON_RIGHT )
            return AKEY_RIGHT;
        if( held & WII_BUTTON_UP || gcHeld & GC_BUTTON_UP )
            return AKEY_UP;
        if( held & WII_BUTTON_DOWN || gcHeld & GC_BUTTON_DOWN )
            return AKEY_DOWN;

        // Return, Escape, Warmstart, Coldstart
        if( down & WII_BUTTON_A || gcDown & GC_BUTTON_A )
            return AKEY_RETURN;
        if( down & WII_BUTTON_B || gcDown & GC_BUTTON_B )
            return AKEY_ESCAPE;
        if( down & WII_BUTTON_COLDSTART || gcDown & GC_BUTTON_COLDSTART )
            return AKEY_COLDSTART;
        if( down & WII_BUTTON_WARMSTART || gcDown & GC_BUTTON_WARMSTART )
            return AKEY_WARMSTART;

        // Exit
        if( down & WII_BUTTON_HOME || gcDown & GC_BUTTON_HOME )
            return AKEY_EXIT;
    }

    if( down & WII_BUTTON_HOME || gcDown & GC_BUTTON_HOME )
        return AKEY_UI;
    /*
	if( down & WII_BUTTON_B || gcDown & GC_BUTTON_B )
		return AKEY_SPACE;
	if( down & WII_BUTTON_A || gcDown & GC_BUTTON_A  )
		return AKEY_RETURN;
	if (new_pad & PAD_L1)
		return AKEY_COLDSTART;
	if (new_pad & PAD_R1)
		return AKEY_WARMSTART;
    */

	if( machine_type == MACHINE_5200 ) 
    {
		if( down & WII_BUTTON_ATARI_START || gcDown & GC_BUTTON_ATARI_START )
			return AKEY_5200_START;
        if( down & WII_BUTTON_5200_ASTERICK || gcDown & GC_BUTTON_5200_ASTERICK )
            return AKEY_5200_ASTERISK;
        if( down & WII_BUTTON_5200_HASH || gcDown & GC_BUTTON_5200_HASH )
            return AKEY_5200_HASH;
        /*
        if( down & WII_BUTTON_5200_PAUSE || gcDown & GC_BUTTON_5200_PAUSE )
            return AKEY_5200_PAUSE;
        if( down & WII_BUTTON_5200_RESET || gcDown & GC_BUTTON_5200_RESET )
            return AKEY_5200_RESET;
        */
	}
	else {
		if( down & WII_BUTTON_ATARI_START || gcDown & GC_BUTTON_ATARI_START )
			key_consol ^= CONSOL_START;
		if( down & WII_BUTTON_ATARI_SELECT || gcDown & GC_BUTTON_ATARI_SELECT )
			key_consol ^= CONSOL_SELECT;
        if( down & WII_BUTTON_ATARI_OPTION || gcDown & GC_BUTTON_ATARI_OPTION )
            key_consol ^= CONSOL_OPTION;
        /*
		if( down & WII_BUTTON_ATARI_HELP || gcDown & GC_BUTTON_ATARI_HELP )
			return AKEY_HELP;
        */
	}

    static int lastkey = AKEY_NONE, key_pressed = 0, key_control = 0, lastmod = 0;
    int shiftctrl = 0;

    SDL_Event event;
    event.type = 0;
    if( SDL_PollEvent( &event ) ) 
    {
        switch( event.type ) 
        {
            case SDL_KEYDOWN:
                lastkey = event.key.keysym.sym;
                lastmod = event.key.keysym.mod;
                key_pressed = 1;
                break;
            case SDL_KEYUP:
                if( event.key.keysym.sym == SDLK_CAPSLOCK )
                {
                    lastkey = SDLK_CAPSLOCK;
                    key_pressed = 1;
                }
                else
                {
                    lastkey = 0;
                    lastmod = 0;
                    key_pressed = 0;                
                }
                break;
            case SDL_QUIT:
                return AKEY_EXIT;
                break;
        }
    }
    else if( !key_pressed )
    {
        return AKEY_NONE;
    }

    if( lastkey == SDLK_CAPSLOCK )
    {           
        if( event.type != SDL_KEYUP && event.type != SDL_KEYDOWN )
        {
            return AKEY_NONE;
        }
        else
        {
            return AKEY_CAPSTOGGLE;                
        }
    }

    if( event.type != SDL_KEYDOWN && event.type != SDL_NOEVENT )
    {
        return AKEY_NONE;
    }

    kbhits = SDL_GetKeyState( NULL );

    if( kbhits == NULL ) 
    {
        Aprint("oops, kbhits is NULL!");
        Aflushlog();
        exit(-1);
    }

#if 0
fprintf( stderr, "E:%x S:%x C:%x K:%x U:%x M:%x\n",event.type,key_shift,key_control,lastkey,event.key.keysym.unicode,event.key.keysym.mod );
#endif

    /* SHIFT STATE */
    if( ( kbhits[SDLK_LSHIFT] ) || ( kbhits[SDLK_RSHIFT] ) || 
        ( lastmod & KMOD_SHIFT ) )
        key_shift = 1;
    else
        key_shift = 0;

    /* CONTROL STATE */
    if( ( kbhits[SDLK_LCTRL] ) || ( kbhits[SDLK_RCTRL ] ) || 
        ( lastmod & KMOD_CTRL ) )
        key_control = 1;
    else
        key_control = 0;

    alt_function = -1;
    if( lastmod & KMOD_ALT ) 
    {
        switch( lastkey ) 
        {
            case SDLK_r:
                alt_function = MENU_RUN;
                break;
            case SDLK_y:
                alt_function = MENU_SYSTEM;
                break;
            case SDLK_o:
                alt_function = MENU_SOUND;
                break;
            case SDLK_w:
                alt_function = MENU_SOUND_RECORDING;
                break;
            case SDLK_a:
                alt_function = MENU_ABOUT;
                break;
            case SDLK_s:
                alt_function = MENU_SAVESTATE;
                break;
            case SDLK_d:
                alt_function = MENU_DISK;
                break;
            case SDLK_l:
                alt_function = MENU_LOADSTATE;
                break;
            case SDLK_c:
                alt_function = MENU_CARTRIDGE;
                break;
            default:
                break;
        }
    }
    if( alt_function != -1 ) 
    {
        key_pressed = 0;
        return AKEY_UI;
    }

    /* OPTION / SELECT / START keys */
    if( kbhits[SDLK_F2] )
    {
        key_consol &= (~CONSOL_OPTION);
    }
    if( kbhits[SDLK_F3] )
    {
        key_consol &= (~CONSOL_SELECT);
    }
    if( kbhits[SDLK_F4] )
    {
        key_consol &= (~CONSOL_START);
    }

    /* Handle movement and special keys. */
    switch( lastkey ) 
    {
        case SDLK_F1:
            key_pressed = 0;
            return AKEY_UI;
        case SDLK_F5:
            key_pressed = 0;
            return key_shift ? AKEY_COLDSTART : AKEY_WARMSTART;
        case SDLK_F8:
            key_pressed = 0;
            return (Atari_Exit(1) ? AKEY_NONE : AKEY_EXIT);
        case SDLK_F9:
            return AKEY_EXIT;
        case SDLK_F10:
            key_pressed = 0;
            return key_shift ? AKEY_SCREENSHOT_INTERLACE : AKEY_SCREENSHOT;
    }

    if( key_shift )
    {
        shiftctrl ^= AKEY_SHFT;
    }

    if( machine_type == MACHINE_5200 && !ui_is_active ) 
    {
        if( lastkey == SDLK_F4 )
            return AKEY_5200_START ^ shiftctrl;

        switch( lastkey ) 
        {
            case 'p':
                return AKEY_5200_PAUSE ^ shiftctrl;
            case 'r':
                return AKEY_5200_RESET ^ shiftctrl;
            case '0':
                return AKEY_5200_0 ^ shiftctrl;
            case '1':
                return AKEY_5200_1 ^ shiftctrl;
            case '2':
                return AKEY_5200_2 ^ shiftctrl;
            case '3':
                return key_shift ? AKEY_5200_HASH : AKEY_5200_3 ^ shiftctrl;
            case '4':
                return AKEY_5200_4 ^ shiftctrl;
            case '5':
                return AKEY_5200_5 ^ shiftctrl;
            case '6':
                return AKEY_5200_6 ^ shiftctrl;
            case '7':
                return AKEY_5200_7 ^ shiftctrl;
            case '8':
                return key_shift ? AKEY_5200_ASTERISK : AKEY_5200_8 ^ shiftctrl;
            case '9':
                return AKEY_5200_9 ^ shiftctrl;
#if 0
            /* XXX: " ^ shiftctrl" harmful for '#' and '*' ? */
            case '#':
            case '=':
                return AKEY_5200_HASH;
            case '*':
                return AKEY_5200_ASTERISK;
#endif
        }

        return AKEY_NONE;
    }

    if( key_control )
    {
        shiftctrl ^= AKEY_CTRL;
    }

    switch( lastkey ) 
    {
        case SDLK_BACKQUOTE: /* fallthrough */
        case SDLK_LSUPER:
            return AKEY_ATARI ^ shiftctrl;
#if 0
        case SDLK_RSUPER:
            if (key_shift)
                return AKEY_CAPSLOCK;
            else
                return AKEY_CAPSTOGGLE;
#endif
        case SDLK_END:
        case SDLK_F6:
            return AKEY_HELP ^ shiftctrl;
        case SDLK_PAGEDOWN:
            return AKEY_F2 | AKEY_SHFT;
        case SDLK_PAGEUP:
            return AKEY_F1 | AKEY_SHFT;
        case SDLK_HOME:
            return AKEY_CLEAR;
        case SDLK_PAUSE:
        case SDLK_F7:
            return AKEY_BREAK;
#if 0
        case SDLK_CAPSLOCK:
            return AKEY_CAPSLOCK;
#endif
        case SDLK_SPACE:
            return AKEY_SPACE ^ shiftctrl;
        case SDLK_BACKSPACE:
            return AKEY_BACKSPACE;
        case SDLK_RETURN:
            return AKEY_RETURN ^ shiftctrl;
        case SDLK_LEFT:
            return AKEY_LEFT ^ shiftctrl;
        case SDLK_RIGHT:
            return AKEY_RIGHT ^ shiftctrl;
        case SDLK_UP:
            return AKEY_UP ^ shiftctrl;
        case SDLK_DOWN:
            return AKEY_DOWN ^ shiftctrl;
        case SDLK_ESCAPE:
            return AKEY_ESCAPE ^ shiftctrl;
        case SDLK_TAB:
            return AKEY_TAB ^ shiftctrl;
        case SDLK_DELETE:
            if (key_shift)
                return AKEY_DELETE_LINE;
            else
                return AKEY_DELETE_CHAR;
        case SDLK_INSERT:
            if (key_shift)
                return AKEY_INSERT_LINE;
            else
                return AKEY_INSERT_CHAR;
    }

    if( key_control )
    {
        switch( lastkey )
        {
            case '.':
                return AKEY_FULLSTOP | AKEY_CTRL;
            case ',':
                return AKEY_COMMA | AKEY_CTRL;
            case ';':
                return AKEY_SEMICOLON | AKEY_CTRL;
            case '0':
                return AKEY_CTRL_0;
            case '1':
                return AKEY_CTRL_1;
            case '2':
                return AKEY_CTRL_2;
            case '3':
                return AKEY_CTRL_3;
            case '4':
                return AKEY_CTRL_4;
            case '5':
                return AKEY_CTRL_5;
            case '6':
                return AKEY_CTRL_6;
            case '7':
                return AKEY_CTRL_7;
            case '8':
                return AKEY_CTRL_8;
            case '9':
                return AKEY_CTRL_9;
            case 'a':
                return AKEY_CTRL_a;
            case 'b':
                return AKEY_CTRL_b;
            case 'c':
                return AKEY_CTRL_c;
            case 'd':
                return AKEY_CTRL_d;
            case 'e':
                return AKEY_CTRL_e;
            case 'f':
                return AKEY_CTRL_f;
            case 'g':
                return AKEY_CTRL_g;
            case 'h':
                return AKEY_CTRL_h;
            case 'i':
                return AKEY_CTRL_i;
            case 'j':
                return AKEY_CTRL_j;
            case 'k':
                return AKEY_CTRL_k;
            case 'l':
                return AKEY_CTRL_l;
            case 'm':
                return AKEY_CTRL_m;
            case 'n':
                return AKEY_CTRL_n;
            case 'o':
                return AKEY_CTRL_o;
            case 'p':
                return AKEY_CTRL_p;
            case 'q':
                return AKEY_CTRL_q;
            case 'r':
                return AKEY_CTRL_r;
            case 's':
                return AKEY_CTRL_s;
            case 't':
                return AKEY_CTRL_t;
            case 'u':
                return AKEY_CTRL_u;
            case 'v':
                return AKEY_CTRL_v;
            case 'w':
                return AKEY_CTRL_w;
            case 'x':
                return AKEY_CTRL_x;
            case 'y':
                return AKEY_CTRL_y;
            case 'z':
                return AKEY_CTRL_z;
        }
    }

    switch( lastkey ) 
    { 
        case 'a':
            return key_shift ? AKEY_A : AKEY_a;
        case 'b':
            return key_shift ? AKEY_B : AKEY_b;
        case 'c':
            return key_shift ? AKEY_C : AKEY_c;
        case 'd':
            return key_shift ? AKEY_D : AKEY_d;
        case 'e':
            return key_shift ? AKEY_E : AKEY_e;
        case 'f':
            return key_shift ? AKEY_F : AKEY_f;
        case 'g':
            return key_shift ? AKEY_G : AKEY_g;
        case 'h':
            return key_shift ? AKEY_H : AKEY_h;
        case 'i':
            return key_shift ? AKEY_I : AKEY_i;
        case 'j':
            return key_shift ? AKEY_J : AKEY_j;
        case 'k':
            return key_shift ? AKEY_K : AKEY_k;
        case 'l':
            return key_shift ? AKEY_L : AKEY_l;
        case 'm':
            return key_shift ? AKEY_M : AKEY_m;
        case 'n':
            return key_shift ? AKEY_N : AKEY_n;
        case 'o':
            return key_shift ? AKEY_O : AKEY_o;
        case 'p':
            return key_shift ? AKEY_P : AKEY_p;
        case 'q':
            return key_shift ? AKEY_Q : AKEY_q;
        case 'r':
            return key_shift ? AKEY_R : AKEY_r;
        case 's':
            return key_shift ? AKEY_S : AKEY_s;
        case 't':
            return key_shift ? AKEY_T : AKEY_t;
        case 'u':
            return key_shift ? AKEY_U : AKEY_u;
        case 'v':
            return key_shift ? AKEY_V : AKEY_v;
        case 'w':
            return key_shift ? AKEY_W : AKEY_w;
        case 'x':
            return key_shift ? AKEY_X : AKEY_x;
        case 'y':
            return key_shift ? AKEY_Y : AKEY_y;
        case 'z':
            return key_shift ? AKEY_Z : AKEY_z;
        case ';':
            return key_shift ? AKEY_COLON : AKEY_SEMICOLON;
        case '0':
            return key_shift ? AKEY_PARENRIGHT : AKEY_0;
        case '1':
            return key_shift ? AKEY_EXCLAMATION : AKEY_1;
        case '2':
            return key_shift ? AKEY_AT : AKEY_2;
        case '3':
            return key_shift ? AKEY_HASH : AKEY_3;
        case '4':
            return key_shift ? AKEY_DOLLAR : AKEY_4;
        case '5':
            return key_shift ? AKEY_PERCENT : AKEY_5;
        case '6':
            return key_shift ? AKEY_CARET : AKEY_6;
        case '7':
            return key_shift ? AKEY_AMPERSAND : AKEY_7;
        case '8':
            return key_shift ? AKEY_ASTERISK : AKEY_8;
        case '9':
            return key_shift ? AKEY_PARENLEFT : AKEY_9;
        case ',':
            return key_shift ? AKEY_LESS : AKEY_COMMA;
        case '.':
            return key_shift ? AKEY_GREATER : AKEY_FULLSTOP;
        case '=':
            return key_shift ? AKEY_PLUS : AKEY_EQUAL;
        case '-':
            return key_shift ? AKEY_UNDERSCORE : AKEY_MINUS;
        case '\'':
            return key_shift ? AKEY_DBLQUOTE : AKEY_QUOTE;
        case '/':
            return key_shift ? AKEY_QUESTION : AKEY_SLASH;
        case '\\':
            return key_shift ? AKEY_BAR : AKEY_BACKSLASH;
        case '[':
            return AKEY_BRACKETLEFT;
        case ']':
            return AKEY_BRACKETRIGHT;
    }

    return AKEY_NONE;
}

void Atari_Initialise(int *argc, char *argv[])
{
    int width, height, bpp;

    width = ATARI_WIDTH;
    height = ATARI_HEIGHT;
    bpp = SDL_ATARI_BPP;
  
    tv_mode = wii_tvmode;

    SetNewVideoMode(width, height, bpp);
    CalcPalette();
    SetPalette();

    Aprint("video initialized");

    SDL_EnableUNICODE(1);

    // Initialize the audio conversion structure
    SDL_BuildAudioCVT(
        &audio_convert,
        AUDIO_U8,
        1,
        48000,
        AUDIO_S16MSB, 
        2,
        48000
        );

    Pokey_sound_init(FREQ_17_EXACT, dsprate, 1, sound_flags);
    InitialiseAudio();
}

int Atari_Exit(int run_monitor)
{
#if 0
    SDL_Quit();
    Aflushlog();
#endif

    return 0;
}

void DisplayWithoutScaling(Uint8 *screen, int jumped, int width)
{
    register Uint32 *start32;
    register int pitch4; 
    int i;

    pitch4 = MainScreen->pitch / 4; 
    //start32 = (Uint32 *) MainScreen->pixels;
    start32 = (Uint32 *) ( MainScreen->pixels + MainScreen->offset ); // WII (offset)

    screen = screen + jumped;
    i = MainScreen->h;
    switch (MainScreen->format->BitsPerPixel) {
    case 8:
        while (i > 0) {
            memcpy(start32, screen, width);
            screen += ATARI_WIDTH;
            start32 += pitch4;
            i--;
        }
        break;
    default:
        Aprint("unsupported color depth %d", MainScreen->format->BitsPerPixel);
        Aprint("please set SDL_ATARI_BPP to 8 or 32 and recompile atari_sdl");
        Aflushlog();
        exit(-1);
    }
}

void DisplayWithScaling(Uint8 *screen, int jumped, int width)
{
    register int x;
    register int dx;
    register int yy;
    register Uint8 *ss;
    register Uint32 *start32;
    int i;
    int y;
    int w1, w2, w4;
    int w, h;
    int pos;
    int pitch4;
    int dy;
    Uint8 c;
    pitch4 = MainScreen->pitch / 4;

    //start32 = (Uint32 *) MainScreen->pixels;
    start32 = (Uint32 *) ( MainScreen->pixels + MainScreen->offset ); // WII (offset)	

    w = (width) << 16;
    h = (ATARI_HEIGHT) << 16;
    dx = w / MainScreen->w;
    dy = h / MainScreen->h;
    w1 = MainScreen->w - 1;
    w2 = MainScreen->w / 2 - 1;
    w4 = MainScreen->w / 4 - 1;
    ss = screen;
    y = (0) << 16;
    i = MainScreen->h;

    switch (MainScreen->format->BitsPerPixel) {
    case 8:
        while( i > 0 )
        {
            x = (width + jumped) << 16;
            pos = w1;
            yy = ATARI_WIDTH * (y >> 16);
            while (pos >= 0) {
                c = ss[yy + (x >> 16)];
                x = x - dx;
                ((Uint8*)start32)[pos] = c;
                pos--;
            }
            start32 += pitch4;
            y = y + dy;
            i--;
        }
        break;
    default:
        Aprint("unsupported color depth %d", MainScreen->format->BitsPerPixel);
        Aprint("please set SDL_ATARI_BPP to 8 or 32 and recompile atari_sdl");
        Aflushlog();
        exit(-1);
    }
}

void Atari_DisplayScreen(void)
{
    int width, jumped;

    switch (WIDTH_MODE) {
    case SHORT_WIDTH_MODE:
        width = ATARI_WIDTH - 2 * 24 - 2 * 8;
        jumped = 24 + 8;
        break;
    case DEFAULT_WIDTH_MODE:
        width = ATARI_WIDTH - 2 * 24;
        jumped = 24;
        break;
    case FULL_WIDTH_MODE:
        width = ATARI_WIDTH;
        jumped = 0;
        break;
    default:
        Aprint("unsupported WIDTH_MODE");
        Aflushlog();
        exit(-1);
        break;
    }

    if (MainScreen->w == width && MainScreen->h == ATARI_HEIGHT) {
        DisplayWithoutScaling((UBYTE *) atari_screen, jumped, width);
    }
    else {
        DisplayWithScaling((UBYTE *) atari_screen, jumped, width);
    }

    SDL_Flip(MainScreen);
}

int Atari_PORT( int num )
{
    if( num == 0 )
    {
	    u8 aret = 0xff;
        u8 bret = 0xff;

	    if( held & WII_BUTTON_ATARI_LEFT || gcHeld & GC_BUTTON_ATARI_LEFT )
		    aret &= 0xf0 | STICK_LEFT;
	    if( held & WII_BUTTON_ATARI_RIGHT || gcHeld & GC_BUTTON_ATARI_RIGHT )
		    aret &= 0xf0 | STICK_RIGHT;
	    if( held & WII_BUTTON_ATARI_UP || gcHeld & GC_BUTTON_ATARI_UP )
		    aret &= 0xf0 | STICK_FORWARD;
	    if( held & WII_BUTTON_ATARI_DOWN || gcHeld & GC_BUTTON_ATARI_DOWN )
		    aret &= 0xf0 | STICK_BACK;

        if( held2 & WII_BUTTON_ATARI_LEFT || gcHeld2 & GC_BUTTON_ATARI_LEFT )
		    bret &= 0xf0 | STICK_LEFT;
	    if( held2 & WII_BUTTON_ATARI_RIGHT || gcHeld2 & GC_BUTTON_ATARI_RIGHT )
		    bret &= 0xf0 | STICK_RIGHT;
	    if( held2 & WII_BUTTON_ATARI_UP || gcHeld2 & GC_BUTTON_ATARI_UP )
		    bret &= 0xf0 | STICK_FORWARD;
	    if( held2 & WII_BUTTON_ATARI_DOWN || gcHeld2 & GC_BUTTON_ATARI_DOWN )
		    bret &= 0xf0 | STICK_BACK;

        return (bret << 4) | (aret & 0x0f);        
    }

	return 0xff;
}

int Atari_TRIG( int num )
{
    u32 h = ( num == 0 ? held : held2 );
    u32 gcH = ( num == 0 ? gcHeld : gcHeld2 );

	if( h & WII_BUTTON_ATARI_FIRE || gcH & GC_BUTTON_ATARI_FIRE )
		return 0;

    return 1;
}

int atari_main(int argc, char **argv)
{
    wii_exit_emulation = 0;

    /* initialise Atari800 core */
    if (!Atari800_Initialise(&argc, argv))
        return 3;

    /* main loop */
    while( !wii_exit_emulation ) 
    {
        key_code = Atari_Keyboard();

        if( wii_exit_emulation )
        {
            break;
        }
        
        Atari800_Frame();
        if( display_screen )
        {
            wii_swap_frame_buffers();
            Atari_DisplayScreen();
            wii_flush_and_sync_video();        
        }        
    }

    return 0;
}
