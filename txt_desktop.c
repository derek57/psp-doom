// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2006 Simon Howard
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

#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomkeys.h"

#include "txt_desktop.h"
#include "txt_gui.h"
#include "txt_io.h"
#include "txt_main.h"
#include "txt_separator.h"
#include "txt_window.h"

#include "i_video.h"
#include "mainmenu.h"

#define MAXWINDOWS 128

static SceCtrlData pad;
static SceCtrlData oldpad;

static char *desktop_title;
static txt_window_t *all_windows[MAXWINDOWS];
static int num_windows = 0;
static int main_loop_running = 0;

static TxtIdleCallback periodic_callback = NULL;
static void *periodic_callback_data;
static unsigned int periodic_callback_period;

void TXT_AddDesktopWindow(txt_window_t *win)
{
    // Previously-top window loses focus:

    if (num_windows > 0)
    {
        TXT_SetWindowFocus(all_windows[num_windows - 1], 0);
    }

    all_windows[num_windows] = win;
    ++num_windows;

    // New window gains focus:

    TXT_SetWindowFocus(win, 1);
}

void TXT_RemoveDesktopWindow(txt_window_t *win)
{
    int from, to;

    // Window must lose focus if it's being removed:

    TXT_SetWindowFocus(win, 0);

    for (from=0, to=0; from<num_windows; ++from)
    {
        if (all_windows[from] != win)
        {
            all_windows[to] = all_windows[from];
            ++to;
        }
    }

    num_windows = to;

    // Top window gains focus:

    if (num_windows > 0)
    {
        TXT_SetWindowFocus(all_windows[num_windows - 1], 1);
    }
}

txt_window_t *TXT_GetActiveWindow(void)
{
    if (num_windows == 0)
    {
        return NULL;
    }

    return all_windows[num_windows - 1];
}

int TXT_RaiseWindow(txt_window_t *window)
{
    int i;

    for (i = 0; i < num_windows - 1; ++i)
    {
        if (all_windows[i] == window)
        {
            all_windows[i] = all_windows[i + 1];
            all_windows[i + 1] = window;

            if (i == num_windows - 2)
            {
                TXT_SetWindowFocus(all_windows[i], 0);
                TXT_SetWindowFocus(window, 1);
            }

            return 1;
        }
    }

    // Window not in the list, or at the end of the list (top) already.

    return 0;
}

int TXT_LowerWindow(txt_window_t *window)
{
    int i;

    for (i = 0; i < num_windows - 1; ++i)
    {
        if (all_windows[i + 1] == window)
        {
            all_windows[i + 1] = all_windows[i];
            all_windows[i] = window;

            if (i == num_windows - 2)
            {
                TXT_SetWindowFocus(window, 0);
                TXT_SetWindowFocus(all_windows[i + 1], 1);
            }

            return 1;
        }
    }

    // Window not in the list, or at the start of the list (bottom) already.

    return 0;
}

static void DrawDesktopBackground(const char *title)
{
    int i;
    unsigned char *screendata;
    unsigned char *p;

    screendata = TXT_GetScreenData();
    
    // Fill the screen with gradient characters

    p = screendata;
    
    for (i=0; i<TXT_SCREEN_W * TXT_SCREEN_H; ++i)
    {
        *p++ = 0xb1;
        *p++ = TXT_COLOR_GREY | (TXT_COLOR_BLUE << 4);
    }

    // Draw the top and bottom banners

    p = screendata;

    for (i=0; i<TXT_SCREEN_W; ++i)
    {
        *p++ = ' ';
        *p++ = TXT_COLOR_BLACK | (TXT_COLOR_GREY << 4);
    }

    p = screendata + (TXT_SCREEN_H - 1) * TXT_SCREEN_W * 2;

    for (i=0; i<TXT_SCREEN_W; ++i)
    {
        *p++ = ' ';
        *p++ = TXT_COLOR_BLACK | (TXT_COLOR_GREY << 4);
    }

    // Print the title

    TXT_GotoXY(0, 0);
    TXT_FGColor(TXT_COLOR_BLACK);
    TXT_BGColor(TXT_COLOR_GREY, 0);

    TXT_PutChar(' ');
    TXT_Puts(title);
}

void TXT_SetDesktopTitle(char *title)
{
    free(desktop_title);
    desktop_title = strdup(title);
    TXT_SetWindowTitle(title);
}

void TXT_DrawDesktop(void)
{
    int i;
    const char *title;

    TXT_InitClipArea();

    if (desktop_title == NULL)
        title = "";
    else
        title = desktop_title;

    DrawDesktopBackground(title);

    for (i=0; i<num_windows; ++i)
    {
        TXT_DrawWindow(all_windows[i]);
    }

    TXT_UpdateScreen();
}

void TXT_DispatchEvents(void)
{
//    int c;

    unsigned long	key;

    oldpad = pad;

    sceCtrlPeekBufferPositive(&pad, 1);

    key = Read_Key();

    if (key & PSP_CTRL_UP)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 173);
    	    }
	}
    }
    else if (key & PSP_CTRL_DOWN)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 9);
    	    }
	}
    }
    else if (key & PSP_CTRL_LEFT)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 91);
    	    }
	}
    }
    else if (key & PSP_CTRL_RIGHT)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 93);
    	    }
	}
    }
    else if (key & PSP_CTRL_CROSS)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 13);
    	    }
	}
    }
    else if (key & PSP_CTRL_SELECT)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], 27);
    	    }
	}
    }
    else if (key & PSP_CTRL_START)
    {
	{
	    if (num_windows > 0)
    	    {
		// Send the keypress to the top window

        	TXT_WindowKeyPress(all_windows[num_windows - 1], (0x80+0x44));
    	    }
	}
    }
}

void TXT_ExitMainLoop(void)
{
    main_loop_running = 0;
}

void TXT_DrawASCIITable(void)
{
    unsigned char *screendata;
    char buf[10];
    int x, y;
    int n;

    screendata = TXT_GetScreenData();

    TXT_FGColor(TXT_COLOR_BRIGHT_WHITE);
    TXT_BGColor(TXT_COLOR_BLACK, 0);

    for (y=0; y<16; ++y)
    {
        for (x=0; x<16; ++x)
        {
            n = y * 16 + x;

            TXT_GotoXY(x * 5, y);
            sprintf(buf, "%02x   ", n);
            TXT_Puts(buf);

            // Write the character directly to the screen memory buffer:

            screendata[(y * TXT_SCREEN_W + x * 5 + 3) * 2] = n;
        }
    }

    TXT_UpdateScreen();
}

void TXT_SetPeriodicCallback(TxtIdleCallback callback,
                             void *user_data,
                             unsigned int period)
{
    periodic_callback = callback;
    periodic_callback_data = user_data;
    periodic_callback_period = period;
}

void TXT_GUIMainLoop(void)
{
    main_loop_running = 1;

    while (main_loop_running)
    {
        TXT_DispatchEvents();

        // After the last window is closed, exit the loop

        if (num_windows <= 0)
        {
//            TXT_ExitMainLoop();
	    MainMenu();
        }

        TXT_DrawDesktop();
/*
        TXT_DrawASCIITable();

        if (periodic_callback == NULL)
        {
            TXT_Sleep(0);
        }
        else
        {
            TXT_Sleep(periodic_callback_period);

            periodic_callback(periodic_callback_data);
        }
*/
    }
}

