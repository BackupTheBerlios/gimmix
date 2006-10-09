#ifndef GIMMIXCORE_H
#define GIMMIXCORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include "config.h"

typedef struct songinfo
{
	char *title;
	char *artist;
	char *album;
	char *genre;
	char *file;
	int length;
	int id;
} SongInfo;

/* mpd control functions */
MpdObj * gimmix_mpd_connect(Conf *);

/* playback control */
int gimmix_play(MpdObj *);
bool gimmix_stop(MpdObj *);
bool gimmix_prev(MpdObj *);
bool gimmix_next(MpdObj *);
int gimmix_seek(MpdObj *, int);

/* voulme control */
int gimmix_get_volume(MpdObj *mo);
void gimmix_set_volume(MpdObj *, int);

/* other functions */
SongInfo * gimmix_get_song_info(MpdObj *);
void gimmix_free_song_info(SongInfo *);
int gimmix_get_progress_status(MpdObj *, float *, char *);
int gimmix_get_total_song_time(MpdObj *);
char * gimmix_get_song_length(SongInfo *);
bool gimmix_is_playing(MpdObj *);

#endif
