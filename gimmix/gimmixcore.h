#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>

/* mpd control functions */
MpdObj * gimmix_mpd_connect(void);
void gimmix_set_volume(MpdObj *, int);

int gimmix_mpd_play (MpdObj *);
int gimmix_mpd_stop (void);
int gimmix_mpd_prev (void);
int gimmix_mpd_next (void);
