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
#include "playlist.h"

#define GLADE_FILE "/share/gimmix/gimmix.glade"

bool
gimmix_connect (void)
{
	MpdObj *mo;
	
	if((mo = gimmix_mpd_connect(pub->conf)))
	{	
		pub->gmo = mo;
		return true;
	}
	return false;
}

void
gimmix_connect_error (void)
{
	GtkWidget 	*error_dialog;
	GtkWidget	*error_label;
	gchar 		*error_markup;

	gchar *error = "ERROR: Couldn't connect to mpd. Check whether mpd is running.\nAlso check that you have specified the proper hostname, port \nand password in ~/.gimmixrc";
	error_markup = g_markup_printf_escaped ("<span size=\"larger\">%s</span>", error);
	error_label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL(error_label), error_markup);
	g_free (error_markup);
	error_dialog = gtk_dialog_new_with_buttons ("Error",
												main_window,
												GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_STOCK_OK,
												GTK_RESPONSE_ACCEPT,
												NULL);
    g_signal_connect (error_dialog,
					"response",
					G_CALLBACK (error_dialog_response),
					(gpointer)error_dialog);
	
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(error_dialog)->vbox), error_label);
    gtk_widget_show_all (error_dialog);
}

void
error_dialog_response (GtkDialog *err_dialog, gint arg1, gpointer dialog)
{
	if (arg1 == GTK_RESPONSE_ACCEPT || arg1 == GTK_RESPONSE_DELETE_EVENT)
	{
		gtk_widget_destroy (GTK_WIDGET(dialog));
		gtk_main_quit ();
	}
}

main (int argc, char *argv[])
{
	GladeXML *xml;
	gchar 	*path;

	pub = (GM *) malloc(sizeof(GM));
	pub->conf = gimmix_config_init ();
	gtk_init (&argc, &argv);
	path = g_strdup_printf ("%s%s", PREFIX, GLADE_FILE);
	xml = glade_xml_new (path, NULL, NULL);
	g_free (path);
	glade_xml_signal_autoconnect(xml);

	main_window = glade_xml_get_widget(xml, "main_window");
	
	if(gimmix_connect())
	{
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
		info_file = glade_xml_get_widget(xml, "info_file");
		info_title = glade_xml_get_widget(xml,"info_title");
		info_artist = glade_xml_get_widget(xml,"info_artist");
		info_album = glade_xml_get_widget(xml,"info_album");
		info_genre = glade_xml_get_widget(xml,"info_genre");
		info_length = glade_xml_get_widget(xml, "info_length");
		
		/* Preferences dialog */
		pref_window = glade_xml_get_widget(xml, "prefs_window");
		host_entry = glade_xml_get_widget(xml,"host_entry");
		port_entry = glade_xml_get_widget(xml,"port_entry");
		password_entry = glade_xml_get_widget(xml,"password_entry");
		systray_toggle = glade_xml_get_widget(xml, "systray_checkbutton");
		button_apply = glade_xml_get_widget(xml, "button_apply");
		
		/* Playlist */
		directory_treeview = glade_xml_get_widget(xml, "album");
		songs_treeview = glade_xml_get_widget(xml, "list");
		current_playlist_treeview = glade_xml_get_widget(xml, "current_playlist_treeview");
		
		gtk_widget_show (main_window);
		gimmix_init ();
	}
	else
	{
		gimmix_connect_error ();
	}

	gtk_main ();
	gimmix_config_save (pub->conf);
	exit_cleanup ();
}

void exit_cleanup ()
{
	if (pub->conf)
		gimmix_config_free (pub->conf);
	if (pub)
		g_free (pub);
	return;
}
