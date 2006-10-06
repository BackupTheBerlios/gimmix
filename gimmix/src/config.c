/*
 * config.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */
 
#include <stdlib.h>
#include <string.h>
#include <confuse.h>
#include "config.h"

Conf * gimmix_config_init(void)
{
	Conf *conf = (Conf*)malloc(sizeof(Conf));
	int enable_systray;
	char *hostname = NULL;
	char *password = NULL;
	
	cfg_opt_t opts[] = {
		CFG_SIMPLE_INT("enable_systray", &enable_systray),
		CFG_SIMPLE_STR("mpd_hostname", &hostname),
		CFG_SIMPLE_STR("mpd_password", &password),
		CFG_END()
	};

	cfg_t *cfg;
	
	cfg = cfg_init(opts, 0);
	if (cfg_parse(cfg, "~/.gimmixrc"))
	{	
		printf("systray status: %d\n", enable_systray);
		printf("hostname: %s\n", hostname);

		conf->systray_enable = enable_systray;
		conf->hostname = hostname;
		conf->password = password;
		return conf;
	}
	return NULL;
}
