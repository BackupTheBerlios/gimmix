/*
 * gimmix-prefs.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * Gimmix is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Gimmix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Gimmix; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */
 
#include "gimmix-prefs.h"
#include "gimmix-systray.h"
#include "gimmix-interface.h"
#include "gimmix.h"

extern MpdObj		*gmo;
extern GladeXML 	*xml;
extern ConfigFile	conf;

static void 	cb_pref_apply_clicked (GtkWidget *widget, gpointer data);
static void		cb_pref_systray_toggled (GtkToggleButton *button, gpointer data);
static void		cb_pref_crossfade_toggled (GtkToggleButton *button, gpointer data);

void
gimmix_prefs_dialog_show (void)
{
	gchar 		*port;
	GtkWidget	*entry;
	GtkWidget	*widget;
	GtkWidget	*pref_window;
	GtkWidget	*c_spin;
	gint		crossfade_time;
	
	pref_window = glade_xml_get_widget (xml, "prefs_window");
	port = g_strdup_printf ("%s", cfg_get_key_value (conf, "mpd_port"));

	gtk_entry_set_text (GTK_ENTRY(glade_xml_get_widget (xml,"host_entry")), cfg_get_key_value (conf, "mpd_hostname"));

	gtk_entry_set_text (GTK_ENTRY(glade_xml_get_widget (xml,"port_entry")), port);
	g_free (port);
	
	entry = glade_xml_get_widget (xml,"password_entry");
	gtk_entry_set_visibility (GTK_ENTRY(entry), FALSE);
	gtk_entry_set_invisible_char (GTK_ENTRY(entry), g_utf8_get_char("*"));
		
	gtk_entry_set_text (GTK_ENTRY(entry), cfg_get_key_value (conf, "mpd_password"));

	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(glade_xml_get_widget (xml, "conf_dir_chooser")), cfg_get_key_value(conf, "music_directory"));
	
	entry = glade_xml_get_widget (xml, "systray_checkbutton");
	if (strncasecmp(cfg_get_key_value(conf, "enable_systray"), "true", 4) == 0)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(entry), TRUE);
	else
	{
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(entry), FALSE);
	}
	g_signal_connect (G_OBJECT(entry), "toggled", G_CALLBACK(cb_pref_systray_toggled), NULL);
	
	widget = glade_xml_get_widget (xml, "pref_play_immediate");
	if (strncasecmp(cfg_get_key_value(conf, "play_on_add"), "true", 4) == 0)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), FALSE);
		
	widget = glade_xml_get_widget (xml, "pref_stop_on_exit");
	if (strncasecmp(cfg_get_key_value(conf, "stop_on_exit"), "true", 4) == 0)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), FALSE);
	
	widget = glade_xml_get_widget (xml, "pref_crossfade");
	c_spin = glade_xml_get_widget (xml, "crossfade_spin");
	g_signal_connect (G_OBJECT(widget), "toggled", G_CALLBACK(cb_pref_crossfade_toggled), c_spin);
	
	crossfade_time = mpd_status_get_crossfade (gmo);
	if (crossfade_time != 0)
	{
		gtk_widget_set_sensitive (GTK_WIDGET(widget), TRUE);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET(glade_xml_get_widget (xml, "crossfade_spin")), TRUE);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(c_spin), (gdouble)crossfade_time);
	}
	else
	{
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET(c_spin), FALSE);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(c_spin), (gdouble)crossfade_time);
	}
	
	g_signal_connect (G_OBJECT(glade_xml_get_widget (xml, "button_apply")), "clicked", G_CALLBACK(cb_pref_apply_clicked), NULL);
	gtk_widget_show (GTK_WIDGET(pref_window));
	
	return;
}

static void
cb_pref_apply_clicked (GtkWidget *widget, gpointer data)
{
	const gchar *host;
	const gchar *port;
	const gchar *password;
	const gchar *dir;
	gint		val;
	GtkWidget 	*pref_widget;

	pref_widget = glade_xml_get_widget (xml,"host_entry");
	host = gtk_entry_get_text (GTK_ENTRY(pref_widget));
	cfg_add_key (&conf, "mpd_hostname", (char *)host);
	
	pref_widget = glade_xml_get_widget (xml,"port_entry");
	port = gtk_entry_get_text (GTK_ENTRY(pref_widget));
	cfg_add_key (&conf, "mpd_port", (char *)port);
	
	pref_widget = glade_xml_get_widget (xml,"password_entry");
	password = gtk_entry_get_text (GTK_ENTRY(pref_widget));
	cfg_add_key (&conf, "mpd_password", (char *)password);
	
	pref_widget = glade_xml_get_widget (xml, "conf_dir_chooser");
	dir = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(pref_widget));
	cfg_add_key (&conf, "music_directory", (char *)dir);
		
	pref_widget = glade_xml_get_widget (xml, "pref_play_immediate");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pref_widget)))
		cfg_add_key (&conf, "play_on_add", "true");
	else
		cfg_add_key (&conf, "play_on_add", "false");
		
	pref_widget = glade_xml_get_widget (xml, "pref_stop_on_exit");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pref_widget)))
		cfg_add_key (&conf, "stop_on_exit", "true");
	else
		cfg_add_key (&conf, "stop_on_exit", "false");
		
	pref_widget = glade_xml_get_widget (xml, "pref_crossfade");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(pref_widget)))
	{
		pref_widget = glade_xml_get_widget (xml, "crossfade_spin");
		gint val = gtk_spin_button_get_value (GTK_SPIN_BUTTON(pref_widget));
		mpd_status_set_crossfade (gmo, val);
	}
	else
	{
		mpd_status_set_crossfade (gmo, 0);
	}
	
	gimmix_config_save ();

	return;
}

static void
cb_pref_crossfade_toggled (GtkToggleButton *button, gpointer data)
{
	gtk_widget_set_sensitive (GTK_WIDGET(data), gtk_toggle_button_get_active (button));
	
	return;
}

static void
cb_pref_systray_toggled (GtkToggleButton *button, gpointer data)
{
	if (gtk_toggle_button_get_active(button) == TRUE)
	{
		gimmix_enable_systray_icon ();
		cfg_add_key (&conf, "enable_systray", "true");
	}
	else
	if (gtk_toggle_button_get_active(button) == FALSE)
	{
		gimmix_disable_systray_icon ();
		cfg_add_key (&conf, "enable_systray", "false");
	}
	
	gimmix_config_save ();

	return;
}

