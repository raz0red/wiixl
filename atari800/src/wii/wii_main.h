#ifndef WII_MAIN_H
#define WII_MAIN_H

#include <gccore.h>
#include <sys/dir.h>
#include <unistd.h>


/*
 * The different types of nodes in the menu
 */
enum NODETYPE
{
    NODETYPE_ROOT,
    NODETYPE_SCALE,
    NODETYPE_SPACER,
    NODETYPE_START_EMULATION,
    NODETYPE_ADVANCED,
    NODETYPE_DEBUG_MODE,
    NODETYPE_BW_MODE,
    NODETYPE_WIDTH_MODE,
    NODETYPE_VSYNC,
    NODETYPE_SYSTEM
};

struct treenode;
typedef struct treenode *TREENODEPTR;

/*
 * Simple hierarchical menu structure
 */
typedef struct treenode
{
    char *name;
    enum NODETYPE node_type;
    TREENODEPTR *children;
    u16 child_count;
    u16 max_children;
} TREENODE;

#define WII_SD_CARD "sd:"
#define WII_BASE_APP_DIR WII_SD_CARD "/apps/wiixl/"
#define WII_FILES_DIR WII_SD_CARD "/wiixl"
#define WII_CONFIG "wiixl.conf"

#define WII_MAX_PATH MAXPATHLEN
#define WII_MENU_BUFF_SIZE 256

// Whether to display debug info (FPS, etc.)
extern short wii_debug;
// The most recent status message
extern char wii_status_message[];
// Hardware buttons (reset, power, etc.)
extern u8 wii_hw_button;
// The stack containing the menus the user has navigated
extern TREENODEPTR wii_menu_stack[4];
// The head of the menu stack
extern s8 wii_menu_stack_head;
// Two framebuffers (double buffered)
extern u32 *wii_xfb[2];

// The scale
extern int wii_scale;

// The frame buffer (from SDL)
extern u32 *frame_buffer;
// The display mode (from SDL)
extern GXRModeObj *display_mode;	

/*
 * Resets the menu indexes when an underlying menu in change (push/pop)
 */
extern void wii_menu_reset_indexes();

/*
 * Move the current selection in the menu by the specified step count
 *
 * menu     The current menu
 * steps    The number of steps to move the selection
 */
extern void wii_menu_move( TREENODE *menu, s16 steps );

/*
 * Pops the latest menu off of the menu stack (occurs when the user leaves 
 * the current menu.
 *
 * return   The pop'd menu
 */
extern TREENODE* wii_menu_pop();

/*
 * Swaps the frame buffers
 */
extern void wii_swap_frame_buffers();

/*
 * Flush and syncs the video 
 */
extern void wii_flush_and_sync_video();

// UI Navigation, Standard Buttons
#define WII_BUTTON_LEFT ( WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_LEFT ( PAD_BUTTON_LEFT )
#define WII_BUTTON_RIGHT ( WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_UP ( WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP  )
#define GC_BUTTON_UP ( PAD_BUTTON_UP )
#define WII_BUTTON_DOWN ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN  )
#define GC_BUTTON_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_A ( WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A )
#define GC_BUTTON_A ( PAD_BUTTON_A )
#define WII_BUTTON_B ( WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B )
#define GC_BUTTON_B ( PAD_BUTTON_B )
#define WII_BUTTON_HOME ( WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME )
#define GC_BUTTON_HOME ( PAD_TRIGGER_Z )

// UI Navigation, Warm and Cold Start
#define WII_BUTTON_COLDSTART ( WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS )
#define GC_BUTTON_COLDSTART ( PAD_TRIGGER_R )
#define WII_BUTTON_WARMSTART ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_BUTTON_WARMSTART ( PAD_TRIGGER_L )

// 5200
#define WII_BUTTON_5200_ASTERICK ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_BUTTON_5200_ASTERICK ( PAD_TRIGGER_L )
#define WII_BUTTON_5200_HASH ( WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_FULL_R )
#define GC_BUTTON_5200_HASH ( PAD_TRIGGER_R )

// Computers
#define WII_BUTTON_ATARI_START ( WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS )
#define GC_BUTTON_ATARI_START ( PAD_BUTTON_START )
#define WII_BUTTON_ATARI_SELECT ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_BUTTON_ATARI_SELECT ( PAD_TRIGGER_L )
#define WII_BUTTON_ATARI_OPTION ( WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_FULL_R )
#define GC_BUTTON_ATARI_OPTION ( PAD_TRIGGER_R )
#define WII_BUTTON_ATARI_FIRE ( WPAD_BUTTON_2 | WPAD_CLASSIC_BUTTON_A )
#define GC_BUTTON_ATARI_FIRE ( PAD_BUTTON_A )
#define WII_BUTTON_ATARI_UP ( WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_UP )
#define GC_BUTTON_ATARI_UP ( PAD_BUTTON_UP )
#define WII_BUTTON_ATARI_DOWN ( WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_DOWN )
#define GC_BUTTON_ATARI_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_ATARI_RIGHT ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_ATARI_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_ATARI_LEFT ( WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_ATARI_LEFT ( PAD_BUTTON_LEFT )

#endif
