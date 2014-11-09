//
// Copyright(C) 2005-2014 Simon Howard
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
// DESCRIPTION:
//    Exit text-mode ENDOOM screen.
//

#include <stdio.h>
#include <string.h>

#include <pspctrl.h>

//#include "config.h"
#include "doomtype.h"
#include "i_video.h"

#include "txt_main.h"

#define ENDOOM_W 80
#define ENDOOM_H 25

// 
// Displays the text mode ending screen after the game quits
//

static SceCtrlData ctrl;
static SceCtrlData lastpad;

void I_Endoom(byte *endoom_data)
{
    unsigned char *screendata;
    int y;
    int indent;

    // Set up text mode screen

    TXT_Init();
//    I_InitWindowTitle();
//    I_InitWindowIcon();

    // Write the data to the screen memory

    screendata = TXT_GetScreenData();

    indent = (ENDOOM_W - TXT_SCREEN_W) / 2;

    for (y=0; y<TXT_SCREEN_H; ++y)
    {
        memcpy(screendata + (y * TXT_SCREEN_W * 2),
               endoom_data + (y * ENDOOM_W + indent) * 2,
               TXT_SCREEN_W * 2);
    }

    // Wait for a keypress

    while (true)
    {
        TXT_UpdateScreen();

//        if (TXT_GetChar() > 0)		// DISABLED FOR PSP CONTROLS
	sceCtrlReadBufferPositive(&ctrl, 1);
	if(ctrl.Buttons != lastpad.Buttons)
	{
	    lastpad = ctrl;
	    if(ctrl.Buttons)
    	    {
    	        break;
    	    }
	}
//        TXT_Sleep(0);				// DISABLED TO PREVENT PSP HANGUP
    }

    // Shut down text mode screen

    TXT_Shutdown();
}

