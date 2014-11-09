// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	System specific network interface stuff.
//
//-----------------------------------------------------------------------------


#ifndef __I_NET__
#define __I_NET__


#ifdef __GNUG__
#pragma interface
#endif

//extern  doomcom_t*	doomcom;

// This points inside doomcom.
//extern  doomdata_t*	netbuffer;	

extern int gameflags,playercolor;
extern int stskill,stmap,stepisode;

#define GF_MULTI 1
#define GF_DEATHMATCH 2
#define GF_RESPAWN 4
#define GF_NOMONSTERS 8
#define GF_SERVER 16

// Called by D_DoomMain.


void I_InitNetwork (void);
void I_NetCmd (void);


#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
