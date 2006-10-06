#include <stdio.h>
#include <stdbool.h>
#include <confuse.h>

typedef struct
{
	int systray_enable;
	char *hostname;
	char *password;
} Conf;

Conf * gimmix_config_init(void);
