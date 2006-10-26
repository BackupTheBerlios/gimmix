#ifndef GIMMIX_H
#define GIMMIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include "config.h"

#define APPNAME "Gimmix"
#define VERSION "0.1 beta"

typedef struct Gimmix
{
	MpdObj *gmo;
	Conf *conf;
} GM;

GM *pub;

bool gimmix_connect (void);

#endif
