#include "gm_mpd.h"

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
}
