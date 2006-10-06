#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include "config.h"

#define APPNAME "Gimmix"
#define VERSION "0.1alpha"

typedef struct Gimmix
{
	MpdObj *gmo;
	Conf *conf;
} GM;

GM *pub;

bool gimmix_connect (void);
