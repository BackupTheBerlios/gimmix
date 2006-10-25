#ifndef GIMMIX_CORE_H
#define GIMMIX_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include "config.h"

/* the SongInfo structure */
typedef struct songinfo
{
	char *title;
	char *artist;
	char *album;
	char *genre;
	char *file;
	int length;
	int id;
	int pos;
} SongInfo;

bool status_is_changed;

/* create a mpd object and connect to mpd using the conf */
MpdObj * gimmix_mpd_connect(Conf *);

/* playback control */
bool gimmix_play(MpdObj *);
bool gimmix_stop(MpdObj *);
bool gimmix_prev(MpdObj *);
bool gimmix_next(MpdObj *);
bool gimmix_seek(MpdObj *, int);

/* voulme control */
int gimmix_get_volume(MpdObj *);
void gimmix_set_volume(MpdObj *, int);

/* Gets the information of currently playing song (artist, title, genre.etc)*/
SongInfo * gimmix_get_song_info(MpdObj *);

/* Free memory allocated by gimmix_get_song_info() */
void gimmix_free_song_info(SongInfo *);

int gimmix_get_progress_status(MpdObj *, float *, char *);
int gimmix_get_total_song_time(MpdObj *);
char * gimmix_get_song_length(SongInfo *);

/* Check whether gimmix is paused/playing or stopped */
int gimmix_is_playing(MpdObj *);

/* Status changed callback */
void status_changed(MpdObj *, ChangedStatusType);

#endif
