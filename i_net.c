#include <pspkerneltypes.h>
#include <pspreg.h>

#include <psptypes.h>
#include <pspwlan.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psputility.h>

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"
#include "v_video.h"
#include "z_zone.h"

#include "doomstat.h"

#ifdef __GNUG__
#pragma implementation "i_net.h"
#endif
#include "i_net.h"

#define VERSION 201

#define ADHOC_PORT 1

#define printf pspDebugScreenPrintf
#define color pspDebugScreenSetTextColor
#define backcolor pspDebugScreenSetBackColor
#define gotoxy pspDebugScreenSetXY

#include <pspnet_adhoc.h>
#include <pspnet_adhocctl.h>

// This is ???
extern  doomcom_t*	doomcom;

// This points inside doomcom.
extern  doomdata_t*	netbuffer;	

typedef struct
{
	int nic;
	u16 port;
	char mac[6];
} sockaddr;


typedef struct serverlist_t
{
	int num;
	char mac[5][25];
	char name[5][128];
} serverlist_t;

sockaddr broadcastaddr,nodeaddr;
sockaddr nodesS[4];

unsigned long myAddr;

int c,len,i,moz,keypressed,stav=0;

serverlist_t serverlist;
sockaddr *saddr;
char *versionchar;
char *versionstart;
char *versionclient;
char *versioncharname;
char *versionclientname;
char *text1;
char *text2;
char tx1[30];
char tx2[30];
char tx3[30];
char tx4[30];
char tx5[30];
int c1,c2,c3,c4;
int socket = -1;
patch_t *sipka;
int netdialog = 0;

char buf[1024];

int pspSdkLoadAdhocModules()
{
	if(sceUtilityLoadNetModule(PSP_NET_MODULE_ADHOC)<0)
	{
		I_Error("Error loading AD HOC module\n");
	}
	if(sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON)<0)
	{
		I_Error("Error loading COMMON module\n");
	}
	if(sceUtilityLoadNetModule(PSP_NET_MODULE_INET)<0)
	{
		I_Error("Error loading INET module");
	}
	return 0;
}

int pspSdkAdhocInit(char *product)
{
	u32 retVal;
	struct productStruct temp;

	retVal = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000);
	if (retVal != 0)
		return retVal;

	retVal = sceNetAdhocInit();
	if (retVal != 0)
		return retVal;

	strcpy(temp.product, product);
	temp.unknown = 0;
	
	retVal = sceNetAdhocctlInit(0x2000, 0x20, &temp);
	if (retVal != 0)
		return retVal;

	return 0;
}

void pspSdkAdhocTerm()
{
	sceNetAdhocctlTerm();
	sceNetAdhocTerm();
	sceNetTerm();
}

int OpenSocket(int port)
{
	char mac[8];
	sceWlanGetEtherAddr(mac);
	int rc = sceNetAdhocPdpCreate(mac, port, 0x2000, 0);
	return rc;
}

int CloseSocket(int socket)
{
	sceNetAdhocPdpDelete(socket, 0);
	return -1;
}

int recv(int socket, char *buf, int len, sockaddr *addr)
{
	int port;
	int datalength = len;
	int ret;
	sceKernelDelayThread(1);
	ret = sceNetAdhocPdpRecv(socket, addr->mac, &port, buf, &datalength, 0, 1);
	if(ret == 0x80410709) return 0; // neblokujici sokety
	if(ret < -1) I_Error("chyba pri prijmani");
	addr->port = port;
	return datalength;
}

int send(int socket, char *buf, int len, sockaddr *addr)
{
	int ret = -1;
	sceKernelDelayThread(1);
	ret = sceNetAdhocPdpSend(socket, addr->mac, addr->port, buf, len, 0, 1);
	if(ret < 0) I_Error("chyba pri odesilani");
	return ret;
}

char *AddrToString(sockaddr *addr)
{
	static char buffer[22];
	sceNetEtherNtostr((unsigned char *)addr->mac, buffer);
	// sprintf(buffer + strlen(buffer)+1, ":%d", addr->port);
	return buffer;
}

int StringToAddr(char *string, sockaddr *addr)
{
	int ha1, ha2, ha3, ha4, ha5, ha6, hp;

	sscanf(string, "%x:%x:%x:%x:%x:%x", &ha1, &ha2, &ha3, &ha4, &ha5, &ha6, &hp);

	addr->mac[0] = ha1 & 0xFF;
	addr->mac[1] = ha2 & 0xFF;
	addr->mac[2] = ha3 & 0xFF;
	addr->mac[3] = ha4 & 0xFF;
	addr->mac[4] = ha5 & 0xFF;
	addr->mac[5] = ha6 & 0xFF;
	addr->port = ADHOC_PORT;

	return 0;
}

int AddrCompare(sockaddr *addr1, sockaddr *addr2)
{
	if((addr1->port != ADHOC_PORT) || (addr2->port != ADHOC_PORT)) return 0;
	if(memcmp(addr1->mac, addr2->mac, 6) != 0) return 0;
	return 1;
}

int netDialog()
{
	int ret, done = 0;

   	pspUtilityNetconfData data;

	memset(&data, 0, sizeof(data));
	data.base.size = sizeof(data);
	data.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	data.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
	data.base.graphicsThread = 17;
	data.base./*unknown*/accessThread = 19;
	data.base.fontThread = 18;
	data.base.soundThread = 16;
	data.action = PSP_NETCONF_ACTION_CONNECT_ADHOC;
//	data.unknown3 = "Doom";

	if(!netdialog) {
		netdialog = 1;
		pspSdkLoadAdhocModules();
		pspSdkAdhocInit("XXDOOMXXX");
	}

	ret = sceUtilityNetconfInitStart(&data);
	if(ret<0) {
		I_Error("netDialog - Chyba 0x%08X",ret);
	}
	
	while(!done)
	{
		I_FinishUpdate(0);
		switch(sceUtilityNetconfGetStatus())
		{
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				break;
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityNetconfUpdate(1);
				break;
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityNetconfShutdownStart();
				break;
			case PSP_UTILITY_DIALOG_FINISHED:
				done = 1;
				break;
			default:
				break;
		}
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

	if(data.base.result) return 0;

	for(i=0; i<6; i++) broadcastaddr.mac[i] = 0xFF;
	broadcastaddr.port = ADHOC_PORT;
	socket = OpenSocket(ADHOC_PORT);

	return 1;
}

int netsend()
{
	send(socket, netbuffer, doomcom->datalength, &nodesS[doomcom->remotenode]);
	//printf("Send: Node %d - %s\n",doomcom->remotenode,AddrToString(&nodesS[doomcom->remotenode]));
	return 0;
}

int netget()
{
	int c=0;
	memset(&nodeaddr,0,sizeof(sockaddr));
	c = recv(socket, netbuffer, 1400, &nodeaddr);
	doomcom->remotenode = -1;
	if(AddrCompare(&nodeaddr,&nodesS[0])) doomcom->remotenode = 0;
	if(AddrCompare(&nodeaddr,&nodesS[1])) doomcom->remotenode = 1;
	if(AddrCompare(&nodeaddr,&nodesS[2])) doomcom->remotenode = 2;
	if(AddrCompare(&nodeaddr,&nodesS[3])) doomcom->remotenode = 3;
	if(doomcom->remotenode == -1) return 0;
	doomcom->datalength = (short)c;
	//printf("Get: Node %d, size %d, %s\n",doomcom->remotenode,doomcom->datalength,AddrToString(&nodeaddr));
	return 0;
}

void I_NetCmd(void)
{
	if (doomcom->command == CMD_SEND)
		netsend();
	else if (doomcom->command == CMD_GET)
		netget();
	else
		I_Error("Bad net cmd: %i\n", doomcom->command);
}

int get_registry_value(const char *dir, const char *name, void *val, int maxlen)
{
	int ret = 0;
	struct RegParam reg;
	REGHANDLE h;

	memset(&reg, 0, sizeof(reg));
	reg.regtype = 1;
	reg.namelen = strlen("/system");
	reg.unk2 = 1;
	reg.unk3 = 1;
	strcpy(reg.name, "/system");
	if(sceRegOpenRegistry(&reg, 2, &h) == 0)
	{
		REGHANDLE hd;
		if(!sceRegOpenCategory(h, dir, 2, &hd))
		{
			REGHANDLE hk;
			unsigned int type, size;

			if(!sceRegGetKeyInfo(hd, name, &hk, &type, &size))
			{
				if(!sceRegGetKeyValue(hd, hk, val, maxlen))
				{
					ret = 1;
					sceRegFlushCategory(hd);
				}
			}
			sceRegCloseCategory(hd);
		}
		sceRegFlushRegistry(h);
		sceRegCloseRegistry(h);
	}

	return ret;
}

char *colorsS[] = {
	"!BUG!",
	"RED",
	"GREEN",
	"BLUE",
	"BROWN",
	"PINK",
	"YELLOW",
	"B & G",
	"GRAY"
};

SceCtrlData pad;

int playercolors[MAXPLAYERS];
int gameflags,playercolor = 72;
int stskill, stmap, stepisode;

char *onoff[] = {
	"NO",
	"YES"
};

void MapMenu()
{
	int moz, kp;
	moz = 0;
	kp = 2;
	while(1) {
		if(!kp) {
			sceCtrlPeekBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_UP) {moz--;kp = 1;}
			if(pad.Buttons & PSP_CTRL_DOWN) {moz++;kp = 1;}
			if(pad.Buttons & PSP_CTRL_CROSS) {
				kp = 2;
				if(moz==0) {
					stepisode++;
					if(stepisode > 4) stepisode = 1;
					if(stepisode !=1 && stmap > 9) stmap = 1;
				}
				if(moz==1) {
					stmap++;
					if(stepisode !=1 && stmap > 9) stmap = 1;
					if(stepisode ==1 && stmap > 32) stmap = 1;
				}
				if(moz==2) {
					stskill++;
					if(stskill > 4) stskill = 0;
				}
				if(moz==3) {
					if(gameflags & GF_NOMONSTERS) gameflags &= ~GF_NOMONSTERS; else gameflags |= GF_NOMONSTERS;
				}
				if(moz==4) return;
			}
			if(moz < 0) moz = 4;
			if(moz > 4) moz = 0;
		}
		if(kp) {
			if(kp == 2) {
				I_ClearPart(0, 0, 80, 319, 199);
				sprintf(text1,"EPISODE: %d",stepisode);
				M_WriteText(70, 80, text1);
				sprintf(text1,"MAP: %d",stmap);
				M_WriteText(70, 100, text1);
				sprintf(text1,"SKILL: %d",stskill);
				M_WriteText(70, 120, text1);
				sprintf(text1,"MONSTERS: %s",onoff[gameflags & GF_NOMONSTERS ? 0 : 1]);
				M_WriteText(70, 140, text1);
				M_WriteText(70, 160, "BACK");
			}
			I_ClearPart(0, 40, 80, 70, 180);
			V_DrawPatchDirect(40, moz*20+80, 0, sipka);
			I_FinishUpdate(1);
			kp = 0;
			sceKernelDelayThread(180*1000);
		}
		sceKernelDelayThread(20*1000);
	}
}

int ServerMenu()
{
	int moz, kp, i;
	moz = 0;
	kp = 2;
	while(1) {
		if(!kp) {
			sceCtrlPeekBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_UP) {moz--;kp = 1;}
			if(pad.Buttons & PSP_CTRL_DOWN) {moz++;kp = 1;}
			if(pad.Buttons & PSP_CTRL_CROSS) {
				kp = 2;
				if(moz==0) {
					if(netDialog()) {
						gameflags |= GF_SERVER;
						gameflags |= GF_MULTI;
						return 1;
					}
				}
				if(moz==1) {
					kp = 2;
					i = playercolor % 10;
					i++;
					if(i>8) i = 1;
					playercolor = (playercolor / 10) * 10 + i;
				}
				if(moz==2) {
					kp = 2;
					i = playercolor / 10;
					i++;
					if(i>8) i = 1;
					playercolor = playercolor % 10 + i * 10;
				}
				if(moz==3) {
					if(gameflags & GF_DEATHMATCH) gameflags &= ~GF_DEATHMATCH; else gameflags |= GF_DEATHMATCH;
				}
				if(moz==4) MapMenu();
				if(moz==5) return 0;
			}
			if(moz < 0) moz = 5;
			if(moz > 5) moz = 0;
		}
		if(kp) {
			if(kp == 2) {
				I_ClearPart(0, 0, 80, 319, 199);
				M_WriteText(70, 80, "START");
				sprintf(text1,"COLOR 1: %s",colorsS[playercolor % 10]);
				M_WriteText(70, 100, text1);
				sprintf(text1,"COLOR 2: %s",colorsS[playercolor / 10]);
				M_WriteText(70, 120, text1);
				sprintf(text1,"DEATHMATCH %s",onoff[gameflags & GF_DEATHMATCH ? 1 : 0]);
				M_WriteText(70, 140, text1);
				M_WriteText(70, 160, "SET MAP");
				M_WriteText(70, 180, "BACK");
			}
			I_ClearPart(0, 40, 80, 70, 199);
			V_DrawPatchDirect(40, moz*20+80, 0, sipka);
			I_FinishUpdate(1);
			kp = 0;
			sceKernelDelayThread(180*1000);
		}
		sceKernelDelayThread(20*1000);
	}
	return 0;
}

int ClientMenu()
{
	int moz, kp;
	moz = 0;
	kp = 2;
	while(1) {
		if(!kp) {
			sceCtrlPeekBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_UP) {moz--;kp = 1;}
			if(pad.Buttons & PSP_CTRL_DOWN) {moz++;kp = 1;}
			if(pad.Buttons & PSP_CTRL_CROSS) {
				if(moz==0) {
					if(netDialog()) {
						gameflags |= GF_MULTI;
						return 1;
					}
				}
				if(moz==1) {
					kp = 2;
					i = playercolor % 10;
					i++;
					if(i>8) i = 1;
					playercolor = (playercolor / 10) * 10 + i;
				}
				if(moz==2) {
					kp = 2;
					i = playercolor / 10;
					i++;
					if(i>8) i = 1;
					playercolor = playercolor % 10 + i * 10;
				}
				if(moz==3) return 0;
			}
			if(moz < 0) moz = 3;
			if(moz > 3) moz = 0;
		}
		if(kp) {
			if(kp == 2) {
				I_ClearPart(0, 0, 80, 319, 199);
				M_WriteText(70, 80, "START");
				sprintf(text1,"COLOR 1: %s",colorsS[playercolor % 10]);
				M_WriteText(70, 100, text1);
				sprintf(text1,"COLOR 2: %s",colorsS[playercolor / 10]);
				M_WriteText(70, 120, text1);
				M_WriteText(70, 140, "BACK");
			}
			I_ClearPart(0, 40, 80, 70, 180);
			V_DrawPatchDirect(40, moz*20+80, 0, sipka);
			I_FinishUpdate(1);
			kp = 0;
			sceKernelDelayThread(180*1000);
		}
		sceKernelDelayThread(20*1000);
	}
	return 0;
}

void MultiGameMenu()
{
	int moz, kp;
	moz = 0;
	kp = 2;
	while(1) {
		if(!kp) {
			sceCtrlPeekBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_UP) {moz--;kp = 1;}
			if(pad.Buttons & PSP_CTRL_DOWN) {moz++;kp = 1;}
			if(moz < 0) moz = 3;
			if(moz > 3) moz = 0;
			if(pad.Buttons & PSP_CTRL_CROSS) {
				if(moz==0) return;
				if(moz==1) {
					kp = 2;
					if(ServerMenu()) return;
				}
				if(moz==2) {
					kp = 2;
					if(ClientMenu()) return;
				}
				if(moz==3) I_Quit();				
			}
		}
		if(kp) {
			if(kp == 2) {
				I_ClearPart(0, 0, 80, 319, 199);
				M_WriteText(70, 80, "SINGLEPLAYER");
				M_WriteText(70, 100, "MULTIPLAYER SERVER");
				M_WriteText(70, 120, "MULTIPLAYER CLIENT");
				M_WriteText(70, 140, "EXIT");
			}
			I_ClearPart(0, 40, 80, 70, 180);
			V_DrawPatchDirect(40, moz*20+80, 0, sipka);
			I_FinishUpdate(1);
			kp = 0;
			sceKernelDelayThread(180*1000);
		}
		sceKernelDelayThread(20*1000);
	}
}

/*
 * I_InitNetwork
 */
void I_InitNetwork(void)
{
	int i;
	char name[128];
	doomcom = malloc(sizeof(*doomcom));
	memset(doomcom, 0, sizeof(*doomcom));
	netbuffer = &doomcom->data;
	doomcom->ticdup = 1;
	doomcom->extratics = 0;

	patch_t *logo;
	I_InitGraphics();
	I_SetPalette (W_CacheLumpName ("PLAYPAL",PU_CACHE));
	I_FinishUpdate(1);
	logo = W_CacheLumpName("M_DOOM", PU_STATIC);
	V_DrawPatchDirect(88, 10, 0, logo);
	Z_Free(logo);
	M_WriteText(130, 65, "START MENU");
	I_FinishUpdate(1);

	text1=malloc(256);
	text2=malloc(256);

	stmap = stepisode = 1;

	sipka = W_CacheLumpName("M_SKULL1", PU_STATIC);
//	MultiGameMenu();

	if (gameflags & GF_MULTI)
	{
	netgame = true;
	if(get_registry_value("/CONFIG/SYSTEM", "owner_name", &name, 128))
	{
		for(i = 0; i < strlen(name); i++) {
			if(name[i]==' ') name[i]='_';
		}
	} else {
		sprintf(name,"Doom");
	}

	versionchar=malloc(128);
	sprintf(versionchar,"doom-%d",VERSION);

	versioncharname=malloc(256);
	sprintf(versioncharname,"doom-%d %s",VERSION,name);

	versionclient=malloc(128);
	sprintf(versionclient,"doom-client-%d",VERSION);

	versionclientname=malloc(256);
	sprintf(versionclientname,"doom-client-%d %s %d",VERSION,name,playercolor);

	versionstart=malloc(128);
	sprintf(versionstart,"doom-start-%d",VERSION);

	moz=0;
	stav=0;
	keypressed = 1;
	sceKernelDelayThread(1000*1000);
	I_ClearPart(0, 0, 80, 319, 199);
	if(gameflags & GF_SERVER) {
		playercolors[0] = playercolor;
		M_WriteText(0, 190, "WAITING FOR CLIENT(S), CROSS TO EXIT");
		I_FinishUpdate(1);
		i = 0;
		while(1) {
			len = recv(socket, versionclientname, 256, &nodeaddr);
			if(len>0) {
				sscanf(versionclientname,"%s %s %d",text1,text2,&playercolor);
				if(strcmp(text1,versionclient)==0) {
					i++;
					playercolors[i] = playercolor;
					nodesS[i]=nodeaddr;
					if(i==1) {
						I_ClearPart(0, 0, 80, 319, 199);
						M_WriteText(0, 190, "CROSS TO START THE GAME, OR WAIT FOR CLIENTS");
					}
					sprintf(text1,"PLAYER %d : %s",i,strupr(text2));
					M_WriteText(0, 100+i*10, text1);
					I_FinishUpdate(1);
					if(i==3) break;
				}
			}
			send(socket,versioncharname,256,&broadcastaddr);
			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_CROSS) break;
				sceKernelDelayThread(10*1000);
		}
		if(i == 0) sceKernelExitGame();
		doomcom->numnodes=i+1;
		i=1;
		sprintf(tx1,"%s",AddrToString(&nodesS[1]));
		sprintf(tx2,"%s",AddrToString(&nodesS[2]));
		sprintf(tx3,"%s",AddrToString(&nodesS[3]));
		while(i!=doomcom->numnodes) {
			sprintf(text2,"%s %s %s %s %d%d%d%d%d%d", versionstart, tx1, tx2, tx3, doomcom->numnodes, i, playercolors[0], playercolors[1], playercolors[2], playercolors[3]);
			send(socket, text2, 256, &nodesS[i]);
			i++;
		}
		sceKernelDelayThread(100*1000);
		doomcom->consoleplayer = 0;
	} else {
		M_WriteText(0, 190, "SEARCHING FOR SERVER(S)");
		keypressed=1;
		serverlist.num = 0;
		while(1) {
			if(serverlist.num<5) len = recv(socket, versioncharname, 256, &nodeaddr);
			if(len) {
				len = 0;
				sscanf(versioncharname,"%s %s",text1,text2);
				if(strcmp(text1,versionchar)==0) {
					text1 = AddrToString(&nodeaddr);
					if(serverlist.num > 0) for(i = 0; i < 6; i++) {
						if(strcmp(serverlist.mac[i],text1)==0) break;
					}
					if(i==6 || serverlist.num == 0) {
						sprintf(serverlist.mac[serverlist.num],text1);
						sprintf(serverlist.name[serverlist.num],text2);
						//gotoxy(0,33);
						//color(0x00FF00);
						//backcolor(0x000000);
						//printf("Found Server - %s",text2);
						keypressed=1;
						serverlist.num++;
					}
				}
			}
			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_CROSS) break;
			if(pad.Buttons & PSP_CTRL_UP) {moz--;keypressed=1;}
			if(pad.Buttons & PSP_CTRL_DOWN) {moz++;keypressed=1;}
			if(moz > serverlist.num) moz = 0;
			if(moz < 0) moz = serverlist.num;
			if(keypressed) {
				keypressed=0;
				M_WriteText(70, 80, "EXIT");
				for(i = 0; i < serverlist.num; i++) {
					sprintf(text1,"%s",strupr(serverlist.name[i]));
					M_WriteText(70, 100+i*20, text1);
				}
				I_ClearPart(0, 40, 80, 70, 180);
				V_DrawPatchDirect(40, moz*20+80, 0, sipka);
				I_FinishUpdate(1);
				sceKernelDelayThread(200*1000);
			}
			sceKernelDelayThread(10*1000);
		}
		if(moz == 0) I_Quit();
		StringToAddr(serverlist.mac[moz-1],&nodesS[0]);
		send(socket, versionclientname, 256, &nodesS[0]);
		I_ClearPart(0, 0, 80, 319, 199);
		sprintf(text1,"CONNECTED TO %s, WAITING FOR GAME START",strupr(serverlist.name[moz-1]));
		M_WriteText(0, 190, text1);
		I_FinishUpdate(1);
		while(1) {
			len = recv(socket, text2, 256, &nodeaddr);
			if(len>0 && AddrCompare(&nodeaddr,&nodesS[0]) && !(strcmp(text2,versionchar)==0)) {
				i = 2991;
				sscanf(text2,"%s %s %s %s %s",text1,tx2,tx3,tx4,tx1);
				if(strcmp(text1,versionstart)==0) {
					playercolors[0] = ((tx1[2]-0x30)*10) + (tx1[3]-0x30);
					playercolors[1] = ((tx1[4]-0x30)*10) + (tx1[5]-0x30);
					playercolors[2] = ((tx1[6]-0x30)*10) + (tx1[7]-0x30);
					playercolors[3] = ((tx1[8]-0x30)*10) + (tx1[9]-0x30);
					doomcom->numnodes=(short)tx1[0]-0x30;
					doomcom->consoleplayer = tx1[1]-0x30;
					if(doomcom->numnodes>1) StringToAddr(tx2,&nodesS[1]);
					if(doomcom->numnodes>2) StringToAddr(tx3,&nodesS[2]);
					if(doomcom->numnodes>3) StringToAddr(tx4,&nodesS[3]);
					break;
				}
			}
			sceKernelDelayThread(10*1000);
		}
	}

/*	for(stmap=0; stmap<4; stmap++) {
		printf("P%d: %d\n",stmap,nodesS[stmap].port);
	}
	sceKernelSleepThread();*/

	doomcom->numplayers = doomcom->numnodes;
	doomcom->deathmatch = gameflags & GF_DEATHMATCH;
	doomcom->skill = stskill;
	doomcom->episode = stepisode;
	doomcom->map = stmap;
	doomcom->savegame = -1;

	deathmatch = gameflags & GF_MULTI && gameflags & GF_SERVER && gameflags & GF_DEATHMATCH;
	if (gameflags & GF_MULTI && gameflags & GF_SERVER) {
		startskill = stskill;
		startepisode = stepisode;
		startmap = stmap;
		autostart = true;
	}

	} else {
		netgame = false;
		doomcom->numplayers = doomcom->numnodes = 1;
		doomcom->deathmatch = false;
		doomcom->consoleplayer = 0;
	}
	I_ClearPart(0, 0, 80, 319, 199);
	M_WriteText(120, 120, "LOADING ...");
	I_FinishUpdate(1);
	R_InitTranslationTables();
	Z_Free(sipka);
	//free(text1);
	//free(text2);
	doomcom->id = DOOMCOM_ID;
	return;
}

void I_DownNet() {
	pspSdkAdhocTerm();
}

