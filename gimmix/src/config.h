#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <confuse.h>

typedef struct
{
	int systray_enable;
	int port;
	char *hostname;
	char *password;
} Conf;

Conf * gimmix_config_init(void);

#endif
