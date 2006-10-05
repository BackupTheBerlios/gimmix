#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>

#define APPNAME "Gimmix"
#define VERSION "0.1alpha"

typedef struct Gimmix
{
	MpdObj *gmo;
} GM;

GM *pub;

int gimmix_init (void);
