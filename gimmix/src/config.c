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
	
	cfg_opt_t opts[] = {
		CFG_SIMPLE_INT("mpd_port", &conf->port),
		CFG_SIMPLE_STR("mpd_hostname", &conf->hostname),
		CFG_SIMPLE_STR("mpd_password", &conf->password),
		CFG_SIMPLE_INT("enable_systray", &conf->systray_enable),
		CFG_END()
	};

	cfg_t *cfg;
	
	cfg = cfg_init(opts, 0);
	cfg_parse(cfg, "~/.gimmixrc");
	
	/* Set default configuration values if reading from config file fails */
	if(!conf->hostname)
		conf->hostname = "localhost";
	if(!conf->port)
		conf->port = 6600;
	if(!conf->systray_enable)
		conf->systray_enable = 1;

	/* Free the memory */
	cfg_free(cfg);

	return conf;
}
