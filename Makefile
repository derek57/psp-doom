TARGET = DOOM
PSPSDK = $(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin

PSP    = yes

BUILD_PRX=1
PSP_FW_VERSION=303

LIBS = -lSDL_mixer -lSDL -lGL -lGLU -lpsprtc -lpspirkeyb -lpsppower -lpspvfpu -lmad -logg -lvorbisidec
LIBS += -lpspaudiocodec -lvorbis -lsmpeg -lpspaudio -lpspgum -lpspgu -lpsphprm -lm -lstdc++ -lcrypto
LIBS += -lpspkubridge -lz #-lSDL_net -lpspwlan -lpspnet -lpspnet_adhoc -lpspsdk -lpspnet_adhocctl
#LIBS += -lpspnet_adhocmatching
CFLAGS = -O2 -g -Wall --fast-math -fno-unit-at-a-time -fdiagnostics-show-option -fno-exceptions
CFLAGS += -G0 -I/usr/local/pspdev/psp/include/SDL

OBJS =											\
am_map.o d_main.o d_items.o d_net.o doomstat.o dstrings.o f_finale.o f_wipe.o g_game.o	\
hu_lib.o hu_stuff.o i_endoom.o info.o m_menu.o m_random.o p_ceilng.o p_doors.o		\
p_enemy.o p_floor.o p_inter.o p_lights.o p_map.o p_maputl.o p_mobj.o p_plats.o p_pspr.o	\
p_saveg.o p_setup.o p_sight.o p_spec.o p_switch.o p_telept.o p_tick.o p_user.o r_bsp.o	\
r_data.o r_draw.o r_main.o r_plane.o r_segs.o r_sky.o r_things.o sounds.o s_sound.o	\
statdump.o st_lib.o st_stuff.o								\
											\
dbopl.o d_event.o d_iwad.o d_loop.o d_mode.o i_main.o i_oplmusic.o			\
i_sdlsound.o i_sound.o i_system.o i_timer.o i_video.o m_argv.o m_bbox.o	m_config.o	\
m_controls.o memio.o m_fixed.o midifile.o m_misc.o mus2mid.o opl.o opl_queue.o		\
opl_sdl.o opl_timer.o sha1.o tables.o txt_io.o txt_sdl.o v_video.o w_checksum.o		\
w_file.o w_file_stdc.o wi_stuff.o w_wad.o z_zone.o					\
											\
deh_ammo.o deh_cheat.o deh_doom.o deh_frame.o deh_io.o deh_main.o deh_mapping.o		\
deh_misc.o deh_ptr.o deh_sound.o deh_str.o deh_text.o deh_thing.o deh_weapon.o		\
											\
xmn_main.o xmn_md5.o xmn_psp.o #mp3.o i_sdlmusic.o					\
											\
#display.o execute.o mainmenu.o mode.o multiplayer.o statdump.o				\
#											\
#txt_button.o txt_checkbox.o txt_desktop.o txt_dropdown.o txt_gui.o			\
#txt_inputbox.o txt_label.o txt_radiobutton.o txt_scrollpane.o txt_separator.o		\
#txt_spinctrl.o txt_strut.o txt_table.o txt_utf8.o txt_widget.o txt_window.o		\
#txt_window_action.o		 							\
#											\
#net_client.o net_common.o net_dedicated.o net_gui.o net_io.o net_loop.o net_packet.o	\
#net_query.o net_sdl.o net_server.o net_structrw.o

OBJS   += disablefpuexceptions.o

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP-DOOM by nitr8 (R1)
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_UNKPNG = PIC0.PNG
PSP_EBOOT_SND0 = SND0.AT3
#PSP_EBOOT_PIC1 = PIC1.PNG

ifeq ($(PSP),yes)
include $(PSPSDK)/lib/build.mak
else

all: DOOM

DOOM: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o DOOM

clean:
	rm -f $(OBJS) DOOM

endif
