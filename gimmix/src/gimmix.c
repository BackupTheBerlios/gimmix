/*
 * gimmix.c
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

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "gimmix.h"
#include "interface.h"

bool gimmix_connect(void)
{
	MpdObj *mo;
	
	if((mo = gimmix_mpd_connect(pub->conf)))
	{	
		pub->gmo = mo;
		return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
	GladeXML *xml;

	pub = (GM *) malloc(sizeof(GM));
	pub->conf = gimmix_config_init();
	
	if(gimmix_connect())
	{
		gtk_init(&argc, &argv);

		xml = glade_xml_new("glade/gimmix.glade", NULL, NULL);
		glade_xml_signal_autoconnect(xml);

		main_window = glade_xml_get_widget(xml, "main_window");

		song_label = glade_xml_get_widget(xml,"song_label");
		artist_label = glade_xml_get_widget(xml,"artist_label");
		album_label = glade_xml_get_widget(xml,"album_label");
		button_play = glade_xml_get_widget(xml,"play_button");
		button_prev = glade_xml_get_widget(xml,"prev_button");
		button_next = glade_xml_get_widget(xml,"next_button");
		button_stop = glade_xml_get_widget(xml,"stop_button");
		button_prefs = glade_xml_get_widget(xml,"pref_button");
		button_info = glade_xml_get_widget(xml,"info_button");
		volume_scale = glade_xml_get_widget(xml,"volume_scale");
		progress = glade_xml_get_widget(xml,"progress");
		progressbox = glade_xml_get_widget(xml,"progress_event_box");

		/* Info dialog */
		info_window = glade_xml_get_widget(xml, "info_window");
		info_title = glade_xml_get_widget(xml,"info_title");
		info_artist = glade_xml_get_widget(xml,"info_artist");
		info_album = glade_xml_get_widget(xml,"info_album");
		info_genre = glade_xml_get_widget(xml,"info_genre");
		
		/* Preferences dialog */
		pref_window = glade_xml_get_widget(xml, "prefs_window");
		host_entry = glade_xml_get_widget(xml,"host_entry");
		port_entry = glade_xml_get_widget(xml,"port_entry");
		password_entry = glade_xml_get_widget(xml,"password_entry");
		systray_toggle = glade_xml_get_widget(xml, "systray_checkbutton");

		gimmix_init();

		gtk_main();
	}

	return(0);
}
