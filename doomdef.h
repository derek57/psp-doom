// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// DESCRIPTION:
//  Internally used data structures for virtually everything,
//   lots of other stuff.
//
//-----------------------------------------------------------------------------

#ifndef __DOOMDEF__
#define __DOOMDEF__

#include <stdio.h>
#include <string.h>

#include "doomtype.h"
#include "i_timer.h"
#include "d_mode.h"

//
// Global parameters/defines.
//
// DOOM version
#define DOOM_VERSION 109

// Version code for cph's longtics hack ("v1.91")
#define DOOM_191_VERSION 111


// If rangecheck is undefined,
// most parameter validation debugging code will not be compiled
#define RANGECHECK

// The maximum number of players, multiplayer/networking.
#define MAXPLAYERS 4

// The current state of the game: whether we are
// playing, gazing at the intermission screen,
// the game final animation, or a demo. 
typedef enum
{
    GS_LEVEL,
    GS_INTERMISSION,
    GS_FINALE,
    GS_DEMOSCREEN,
} gamestate_t;

typedef enum
{
    ga_nothing,
    ga_loadlevel,
    ga_newgame,
    ga_loadgame,
    ga_savegame,
    ga_playdemo,
    ga_completed,
    ga_victory,
    ga_worlddone,
    ga_screenshot
} gameaction_t;

//
// Difficulty/skill settings/filters.
//

// Skill flags.
#define	MTF_EASY		1
#define	MTF_NORMAL		2
#define	MTF_HARD		4

// Deaf monsters/do not react to sound.
#define	MTF_AMBUSH		8


//
// Key cards.
//
typedef enum
{
    it_bluecard,
    it_yellowcard,
    it_redcard,
    it_blueskull,
    it_yellowskull,
    it_redskull,
    
    NUMCARDS
    
} card_t;



// The defined weapons,
//  including a marker indicating
//  user has not changed weapon.
typedef enum
{
    wp_fist,
    wp_pistol,
    wp_shotgun,
    wp_chaingun,
    wp_missile,
    wp_plasma,
    wp_bfg,
    wp_chainsaw,
    wp_supershotgun,

    NUMWEAPONS,
    
    // No pending weapon change.
    wp_nochange

} weapontype_t;


// Ammunition types defined.
typedef enum
{
    am_clip,	// Pistol / chaingun ammo.
    am_shell,	// Shotgun / double barreled shotgun.
    am_cell,	// Plasma rifle, BFG.
    am_misl,	// Missile launcher.
    NUMAMMO,
    am_noammo	// Unlimited for chainsaw / fist.	

} ammotype_t;


// Power up artifacts.
typedef enum
{
    pw_invulnerability,
    pw_strength,
    pw_invisibility,
    pw_ironfeet,
    pw_allmap,
    pw_infrared,
    NUMPOWERS
    
} powertype_t;



//
// Power up durations,
//  how many seconds till expiration,
//  assuming TICRATE is 35 ticks/second.
//
typedef enum
{
    INVULNTICS	= (30*TICRATE),
    INVISTICS	= (60*TICRATE),
    INFRATICS	= (120*TICRATE),
    IRONTICS	= (60*TICRATE)
    
} powerduration_t;

#define MAXPATH		0x108

#define SavePathRoot1 "savegames/"
#define SavePathRoot2 "savegames/doom.wad/"
#define SavePathRoot3 "savegames/doom2.wad/"
#define SavePathRoot4 "savegames/tnt.wad/"
#define SavePathRoot5 "savegames/plutonia.wad/"
#define SavePathRoot6 "savegames/chex.wad/"
#define SavePathRoot7 "savegames/hacx.wad/"
#define SavePathRoot8 "savegames/freedoom.wad/"
#define SavePathBeta14 "savegames/doom.wad/DM_B_14/"
#define SavePathBeta15 "savegames/doom.wad/DM_B_15/"
#define SavePathBeta16 "savegames/doom.wad/DM_B_16/"
#define SavePathShare10 "savegames/doom.wad/DM_S_10/"
#define SavePathShare11 "savegames/doom.wad/DM_S_11/"
#define SavePathShare12 "savegames/doom.wad/DM_S_12/"
#define SavePathShare125 "savegames/doom.wad/DM_S_125/"
#define SavePathShare1666 "savegames/doom.wad/DM_S_16/"
#define SavePathShare18 "savegames/doom.wad/DM_S_18/"
#define SavePathReg11 "savegames/doom.wad/DM_R_11/"
#define SavePathReg12 "savegames/doom.wad/DM_R_12/"
#define SavePathReg16 "savegames/doom.wad/DM_R_16/"
#define SavePathReg18 "savegames/doom.wad/DM_R_18/"
#define SavePathReg19 "savegames/doom.wad/DM_R_19U/"
#define SavePath2Reg1666 "savegames/doom2.wad/DM2_R_16/"
#define SavePath2Reg1666G "savegames/doom2.wad/D2_R_16G/"
#define SavePath2Reg17 "savegames/doom2.wad/DM2_R_17/"
#define SavePath2Reg18F "savegames/doom2.wad/DM2_R_18/"
#define SavePath2Reg19 "savegames/doom2.wad/DM2_R_19/"
#define SavePathTNT191 "savegames/tnt.wad/FD1_1_19/"
#define SavePathTNT192 "savegames/tnt.wad/FD1_2_19/"
#define SavePathPLUT191 "savegames/plutonia.wad/FD2_1_19/"
#define SavePathPLUT192 "savegames/plutonia.wad/FD2_2_19/"
#define SavePathChex "savegames/chex.wad/"
/*
#define SavePathHacxShare10 "savegames/hacx.wad/FD2_2_19/"
#define SavePathHacxReg10 "savegames/hacx.wad/FD2_2_19/"
#define SavePathHacxReg11 "savegames/hacx.wad/FD2_2_19/"
*/
#define SavePathHacxReg12 "savegames/hacx.wad/FD2_2_19/"
/*
#define SavePathFreedoom064 "savegames/freedoom.wad/FRDM064/"
#define SavePathFreedoom07RC1 "savegames/freedoom.wad/FRDM07R1/"
#define SavePathFreedoom07 "savegames/freedoom.wad/FRDM07/"
#define SavePathFreedoom08B1 "savegames/freedoom.wad/FRDM08B1/"
#define SavePathFreedoom08 "savegames/freedoom.wad/FRDM08/"
#define SavePathFreedoom08P1 "savegames/freedoom.wad/FRDM08P1/"
*/
#define SavePathFreedoom08P2 "savegames/freedoom.wad/FRDM08P2/"

#ifndef VER_ID
#define VER_ID "DVL"
#endif

#ifdef RANGECHECK
#define DOOM_VERSIONTEXT "Version 2 +R "__DATE__" ("VER_ID")"
#else
#define DOOM_VERSIONTEXT "Version 2 "__DATE__" ("VER_ID")"
#endif

#ifdef RANGECHECK
#define VERSIONTEXT "V2 +R ("VER_ID") "__TIME__""
#endif

#define YEAR ((/*((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
               +*/ (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define MONTH (__DATE__ [2] == 'n' ? 0                                 \
               : __DATE__ [2] == 'b' ? 1                               \
               : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 2 : 3)   \
               : __DATE__ [2] == 'y' ? 4                               \
               : __DATE__ [2] == 'n' ? 5                               \
               : __DATE__ [2] == 'l' ? 6                               \
               : __DATE__ [2] == 'g' ? 7                               \
               : __DATE__ [2] == 'p' ? 8                               \
               : __DATE__ [2] == 't' ? 9                               \
               : __DATE__ [2] == 'v' ? 10 : 11)

#define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10       \
             + (__DATE__ [5] - '0'))

#define DATE_AS_INT (((YEAR - 2000) * 12 + MONTH) * 31 + DAY)

#define	NUMKEYS		256
#define SCREENWIDTH	320
#define SCREENHEIGHT	200
#define LOADING_DISK_W	16
#define LOADING_DISK_H	16

typedef enum 
{
    DEFAULT_INT,
    DEFAULT_INT_HEX,
    DEFAULT_STRING,
    DEFAULT_FLOAT,
    DEFAULT_KEY,
} default_type_t;

typedef struct
{
    // Name of the variable
    char *name;

    // Pointer to the location in memory of the variable
    /*void*/int *location;

    // Type of the variable
    default_type_t type;

    // If this is a key value, the original integer scancode we read from
    // the config file before translating it to the internal key value.
    // If zero, we didn't read this value from a config file.
    int untranslated;

    // The value we translated the scancode into when we read the 
    // config file on startup.  If the variable value is different from
    // this, it has been changed and needs to be converted; otherwise,
    // use the 'untranslated' value.
    int original_translated;

    // If true, this config variable has been bound to a variable
    // and is being used.
    boolean bound;
} default_t;

typedef struct
{
    default_t *defaults;
    int numdefaults;
    char *filename;
} default_collection_t;

typedef struct
{
    // 0 = no cursor here, 1 = ok, 2 = arrows ok
    short	status;
    
    char	name[10];
    
    // choice = menu item #.
    // if status = 2,
    //   choice=0:leftarrow,1:rightarrow
    void	(*routine)(int choice);
    
    // hotkey in menu
    char	alphaKey;			
} menuitem_t;

typedef struct menu_s
{
    short		numitems;	// # of menu items
    struct menu_s*	prevMenu;	// previous menu
    menuitem_t*		menuitems;	// menu items
    void		(*routine)();	// draw routine
    short		x;
    short		y;		// x,y of menu
    short		lastOn;		// last item user was on in menu
} menu_t;

boolean			gamekeydown[NUMKEYS]; 
boolean			game_quit;
boolean			display_ticker;
boolean			am_rotate;
boolean			wipe;
boolean			error_detected;
boolean			print_resource_pwad_error;
boolean			is_chex_2;

extern boolean		dont_move_forwards;
extern boolean		dont_move_backwards;

extern int		fps;
extern int		fsize;
extern int		fsizerw;
extern int		fsizecq;
extern int		followplayer;
extern int		drawgrid;
extern int		button_layout;
extern int		show_deh_loading_message;

char			extra_wad[256];
char			dehacked_file[256];
char			target[MAXPATH];
char			path_tmp[MAXPATH];
char			temp[MAXPATH];

int			load_extra_wad;
int			load_dehacked;
int			detailLevel;

byte			*tinttable;			// used for translucent sprites

FILE			*debugfile;

void M_WriteText(int x, int y, char *string);
void DrawWorldTimer(void);

#endif          // __DOOMDEF__
