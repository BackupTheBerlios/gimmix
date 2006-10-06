#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>

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
MpdObj * gimmix_mpd_connect(void);

/* playback control */
int gimmix_mpd_play (MpdObj *);
int gimmix_mpd_stop (MpdObj *);
int gimmix_mpd_prev (MpdObj *);
int gimmix_mpd_next (MpdObj *);
int gimmix_seek(MpdObj *, int);

/* voulme control */
int gimmix_get_volume(MpdObj *mo);
void gimmix_set_volume(MpdObj *, int);

/* other functions */
SongInfo * gimmix_get_song_info(MpdObj *);
SongInfo * gimmix_get_full_song_info(MpdObj *);
int gimmix_get_progress_status(MpdObj *, float *, char *);
int gimmix_get_total_song_time(MpdObj *);
bool gimmix_is_playing(MpdObj *);

