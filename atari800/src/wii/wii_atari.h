#ifndef WII_ATARI_H
#define WII_ATARI_H

extern int BW;
extern int WIDTH_MODE;
extern int wii_exit_emulation;
extern int wii_vsync;
extern int wii_tvmode;
extern int atari_main( int argc, char **argv );

#define SHORT_WIDTH_MODE 0
#define DEFAULT_WIDTH_MODE 1
#define FULL_WIDTH_MODE 2

#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1
#define VSYNC_DOUBLE_BUFF 2

#endif
