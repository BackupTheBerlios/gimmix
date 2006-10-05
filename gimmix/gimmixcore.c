/*
 * gimmixcore.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */

#include "gimmixcore.h"

MpdObj * gimmix_mpd_connect(void)
{
	MpdObj *mo;
	mo = mpd_new_default();
	mpd_connect(mo);

	if(mpd_check_connected(mo))
	{	
		fprintf(stdout, "Connected...\n");
		return mo;
	}

	fprintf(stderr, "Error! Couldn't connect to mpd.\n\
					Check whether mpd is running.\n");
	return NULL;
}

int gimmix_play(MpdObj *mo)
{
	if(mpd_playlist_get_playlist_length(mo))
	{
		int state;

		state = mpd_player_get_state(mo);

		if (state == MPD_PLAYER_PAUSE || state == MPD_PLAYER_STOP)
		{	
			mpd_player_play(mo);
			return 0;
		}

		else if(state == MPD_PLAYER_PLAY)
		{	
			mpd_player_pause(mo);
			return 1;
		}
	}
	return -1;
}

int gimmix_stop(MpdObj *mo)
{
	int state;
	
	state = mpd_player_get_state(mo);
	
	if (state == MPD_PLAYER_PAUSE || state == MPD_PLAYER_PLAY)
	{
		mpd_player_stop(mo);
		return 0;
	}
	return 1;
}

int gimmix_prev(MpdObj *mo)
{
	/* does nothing for now */
	return 0;
}

int gimmix_next(MpdObj *mo)
{
	/* does nothing for now */
	return 0;
}

void gimmix_set_volume(MpdObj *mo, int vol)
{
	mpd_status_set_volume(mo, vol);
}

SongInfo * gimmix_get_song_info(MpdObj *mo)
{
	mpd_Song *ms;
	SongInfo *si = (SongInfo *) malloc(sizeof(SongInfo));

	ms = mpd_playlist_get_current_song(mo);
	si->song_name = ms->title;
	si->artist_name = ms->artist;
	si->album_name = ms->album;

	return si;
}

