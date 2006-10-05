#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>

/* mpd control functions */
MpdObj * gimmix_mpd_connect(void);

void gimmix_mpd_play (MpdObj *);
void gimmix_mpd_stop (MpdObj *);
void gimmix_mpd_prev (MpdObj *);
void gimmix_mpd_next (MpdObj *);
