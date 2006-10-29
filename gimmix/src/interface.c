/*
 * interface.c
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
 * Author: Priyank Gosalia <priyankmgg@gmail.com>
 */

#include <glib.h>
#include <libnotify/notify.h>
#include "interface.h"
#include "playlist.h"
#include "gimmix.h"

enum {	PLAY,
		PAUSE,
		STOP
	};

void
gimmix_init (void)
{
	g_signal_connect (G_OBJECT(button_play), "clicked", G_CALLBACK(on_play_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prev), "clicked", G_CALLBACK(on_prev_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_next), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_stop), "clicked", G_CALLBACK(on_stop_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prefs), "clicked", G_CALLBACK(on_prefs_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_info), "clicked", G_CALLBACK(on_info_button_clicked), NULL);
	g_signal_connect(G_OBJECT(volume_scale), "value_changed", G_CALLBACK(on_volume_scale_changed), NULL);
	g_signal_connect (G_OBJECT(volume_scale), "scroll_event", G_CALLBACK(gimmix_scroll_volume_slider), NULL);
	g_signal_connect (G_OBJECT(progressbox), "button_press_event", G_CALLBACK(gimmix_progress_seek), NULL);
	g_signal_connect (G_OBJECT(button_apply), "clicked", G_CALLBACK(on_preferences_apply), NULL);

	volume_adj = gtk_range_get_adjustment(GTK_RANGE(volume_scale));
	if (pub->conf->systray_enable == 1)
	{	
		gimmix_systray_icon_create ();
		notify = gimmix_notify_init (tray_icon);
	}
	gtk_adjustment_set_value (GTK_ADJUSTMENT(volume_adj), gimmix_get_volume(pub->gmo));

	if (gimmix_is_playing(pub->gmo) == PLAY)
	{
		gimmix_set_song_info();
	}
	else
	{	
		gimmix_show_ver_info();
	}
	g_timeout_add (300, gimmix_timer, NULL);
	gimmix_playlist_populate ();
	gimmix_current_playlist_init ();
	gimmix_update_current_playlist ();
}

gboolean
gimmix_timer (void)
{
	gchar time[15];
	gint state;
	float fraction;

	state = gimmix_is_playing (pub->gmo);
	if (state == PLAY || state == PAUSE)
	{
		gimmix_get_progress_status (pub->gmo, &fraction, time);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), fraction);
		gtk_progress_bar_set_text (GTK_PROGRESS_BAR(progress), time);
		if (state == PAUSE)
		{
			GtkWidget *image = get_image ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
			gtk_button_set_image (GTK_BUTTON(button_play), image);
		}
		else
		{
			GtkWidget *image = get_image ("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
			gtk_button_set_image (GTK_BUTTON(button_play), image);
		}
		if (status_is_changed)
			gimmix_set_song_info ();
		return TRUE;
	}
	
	else if(state == STOP)
	{
		GtkWidget *image = get_image ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON(button_play), image);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.0);
		gtk_progress_bar_set_text (GTK_PROGRESS_BAR(progress), "Stopped");
		return TRUE;
	}
	return TRUE;
}

void
on_prev_button_clicked (GtkWidget *widget, gpointer data)
{
	if (gimmix_prev(pub->gmo))
		gimmix_set_song_info();
}

void
on_next_button_clicked (GtkWidget *widget, gpointer data)
{
	if (gimmix_next(pub->gmo))
		gimmix_set_song_info ();
}

void
on_play_button_clicked (GtkWidget *widget, gpointer data)
{
	if (gimmix_play(pub->gmo))
		gimmix_set_song_info ();
}

void
on_stop_button_clicked (GtkWidget *widget, gpointer data)
{
	GtkWidget *image;
	
	if(gimmix_stop(pub->gmo))
	{
		image = get_image("gtk-media-play", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON(button_play), image);
		gimmix_show_ver_info();
	}
	return;
}

void
on_prefs_button_clicked (GtkWidget *widget, gpointer data)
{
	Conf *conf;
	gchar port[8];
	gint systray_enable;

	conf = pub->conf;
	sprintf (port, "%d", conf->port);
	systray_enable = conf->systray_enable;

	gtk_entry_set_text (GTK_ENTRY(host_entry), conf->hostname);
	gtk_entry_set_text (GTK_ENTRY(port_entry), port);
	if(conf->password)
		gtk_entry_set_text (GTK_ENTRY(password_entry), conf->password);
	if(systray_enable == 1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(systray_toggle), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(systray_toggle), FALSE);
	g_signal_connect (G_OBJECT(systray_toggle), "toggled", G_CALLBACK(on_systray_checkbox_toggled), NULL);
	gtk_widget_show(GTK_WIDGET(pref_window));
}

void
on_info_button_clicked (GtkWidget *widget, gpointer data)
{
	gint state;
	state = gimmix_is_playing (pub->gmo);
	
	if (state == PLAY || state == PAUSE)
	{
		SongInfo *info = NULL;
		gchar *length;
		gchar *bitrate;
		info = gimmix_get_song_info (pub->gmo);

		gtk_entry_set_text (GTK_ENTRY(info_file), info->file ? info->file : NULL);
		gtk_label_set_text (GTK_LABEL(info_title), info->title ? info->title : NULL);
		gtk_label_set_text (GTK_LABEL(info_artist), info->artist ? info->artist : NULL);
		gtk_label_set_text (GTK_LABEL(info_album), info->album ? info->album : NULL);
		length = gimmix_get_song_length (info);
		if (length)
		{
			gtk_label_set_text (GTK_LABEL(info_length), length);
			g_free (length);
		}
		bitrate = gimmix_get_song_bitrate (info);
		if (bitrate)
		{
			gtk_label_set_text (GTK_LABEL(info_bitrate), bitrate);
			g_free (bitrate);
		}
		else
			gtk_label_set_text (GTK_LABEL(info_bitrate), "(only available while playing)");
		gtk_label_set_text (GTK_LABEL(info_genre), info->genre ? info->genre : NULL);

		gimmix_free_song_info (info);

		gtk_widget_show (GTK_WIDGET(info_window));
	}
}

void
on_preferences_apply (GtkWidget *widget, gpointer data)
{
	const gchar *host;
	const gchar *port;
	const gchar *password;

	host = gtk_entry_get_text (GTK_ENTRY(host_entry));
	port = gtk_entry_get_text (GTK_ENTRY(port_entry));
	password = gtk_entry_get_text (GTK_ENTRY(password_entry));

	pub->conf->hostname = host;
	pub->conf->password = password;
	pub->conf->port = atoi (port);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(systray_toggle)))
		pub->conf->systray_enable = 1;
	else
		pub->conf->systray_enable = 0;
	gimmix_config_save (pub->conf);
}

void
on_volume_scale_changed (GtkWidget *widget, gpointer data)
{
	gint value;

	value = gtk_adjustment_get_value (GTK_ADJUSTMENT(volume_adj));
	gimmix_set_volume (pub->gmo, value);
}

void
gimmix_scroll_volume_slider (GtkWidget *object, GdkEventScroll *event)
{
	gint volume;
	if (event->type != GDK_SCROLL)
		return;

	switch(event->direction)
	{
		case GDK_SCROLL_UP:
			volume = gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj)) + 2;
			gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj), volume);
			break;
		case GDK_SCROLL_DOWN:
			volume = gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj)) - 2;
			gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj), volume);
			break;
		default:
			return;
	}
}

void
gimmix_progress_seek (GtkWidget *progressbox, GdkEvent *event)
{
	GtkAllocation allocation;
	gint x, newtime, totaltime;
	gdouble seektime;

	x = event->button.x;
	allocation = GTK_WIDGET (progressbox)->allocation;
	totaltime = gimmix_get_total_song_time (pub->gmo);
	seektime = (gdouble)x/allocation.width;
	newtime = seektime * totaltime;
	if (gimmix_seek(pub->gmo, newtime))
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), seektime);
	return;
}

void
gimmix_window_visible (void)
{
	static int x;
	static int y;

	if( !GTK_WIDGET_VISIBLE (main_window) )
	{	
		gtk_window_move (GTK_WINDOW(main_window), x, y);
		gtk_widget_show (GTK_WIDGET(main_window));
	}
	else
	{	
		gtk_window_get_position (GTK_WINDOW(main_window), &x, &y);
		gtk_widget_hide (GTK_WIDGET(main_window));
	}
	return;
}

GtkWidget *
get_image (const gchar *id, GtkIconSize size)
{
	GtkWidget *image;

	image = gtk_image_new_from_stock (id, size);
	return image;
}

void
gimmix_set_song_info (void)
{
	gchar *markup;
	gchar title[80];
	SongInfo *song = NULL;
	
	song = gimmix_get_song_info (pub->gmo);

	if( song->title)
	{
		g_sprintf (title, "Gimmix - %s", song->title);
		gtk_window_set_title (GTK_WINDOW(main_window), title);
		markup = g_markup_printf_escaped ("<span size=\"medium\"weight=\"bold\"><i>%s</i></span>", song->title);
		gtk_label_set_markup (GTK_LABEL(song_label), markup);
	}
	else
	{
		markup = g_markup_printf_escaped ("<span size=\"medium\"weight=\"bold\"><i>%s</i></span>", g_path_get_basename(song->file));
		gtk_label_set_markup (GTK_LABEL(song_label), markup);
		gtk_window_set_title (GTK_WINDOW(main_window), "Gimmix");
	}

	if (song->artist)
		gtk_label_set_text (GTK_LABEL(artist_label), song->artist);
	else
		gtk_label_set_text (GTK_LABEL(artist_label), NULL);
	if (song->album)
		gtk_label_set_text (GTK_LABEL(album_label), song->album);
	else
		gtk_label_set_text (GTK_LABEL(album_label), NULL);

	g_free (markup);

	gimmix_free_song_info (song);
}

void
gimmix_systray_icon_create (void)
{
	/* gchar *icon_tooltip = "Gimmix";
	gtk_status_icon_set_tooltip(tray_icon, icon_tooltip);*/
	tray_icon = gtk_status_icon_new_from_stock("gtk-cdrom");
	g_signal_connect (tray_icon, "popup-menu", G_CALLBACK (gimmix_systray_popup_menu), NULL);
	g_signal_connect (tray_icon, "activate", G_CALLBACK(gimmix_window_visible), NULL);
}

void
gimmix_systray_popup_menu (void)
{
	GtkWidget *menu, *menu_item;

	menu = gtk_menu_new();

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_about_show), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_PLAY, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (on_play_button_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_STOP, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (on_stop_button_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (on_prev_button_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_NEXT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (on_next_button_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gtk_main_quit), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	gtk_widget_show (menu);
	gtk_menu_popup (GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,gtk_get_current_event_time());
}

NotifyNotification *
gimmix_notify_init (GtkStatusIcon *status_icon)
{
	NotifyNotification *notify;
	
	/* Initialize notify */
	if(!notify_is_initted())
		notify_init("Gimmix");
	
	notify = notify_notification_new_with_status_icon (APPNAME, APPNAME, "gtk-cdrom", status_icon);
	
	notify_notification_set_timeout (notify, 1000);
	//notify_notification_show(notify, NULL);
	return notify;
}

void 
on_systray_checkbox_toggled (GtkWidget *widget, gpointer data)
{
	if(pub->conf->systray_enable == 1)
	{	
		pub->conf->systray_enable = 0;
		g_object_unref (G_OBJECT(notify));
		g_object_unref (tray_icon);
		return;
	}
	else
	{
		pub->conf->systray_enable = 1;
		gimmix_systray_icon_create ();
		notify = gimmix_notify_init (tray_icon);
	}
	return;
}

void
gimmix_about_show (void)
{
    GdkPixbuf 	*about_pixbuf;
	gchar		*path;
	
	path = g_strdup_printf ("%s%s", PREFIX, "/share/pixmaps/gimmix.png");
    about_pixbuf = gdk_pixbuf_new_from_file (path, NULL);
    g_free (path);

	gchar *website = "http://priyank.one09.net/";
	gchar *website_label = "http://priyank.one09.net/";
	gchar *authors[] = 	{ "Priyank M. Gosalia <priyankmg@gmail.com>",
						 "Part of the song seek code borrowed from Pygmy.",
						 NULL
						};
	
	gtk_show_about_dialog (NULL,
                           "name", APPNAME,
                           "version", VERSION,
                           "copyright", "\xC2\xA9 2006 Priyank Gosalia  (GPL)",
                           "comments", "Gimmix is a GUI MPD client written in C",
                           "authors", authors,
                           "website", website,
                           "website-label", website_label,
                           "logo", about_pixbuf, 
                           NULL);
	return;
}

void
gimmix_show_ver_info (void)
{
	gchar *markup;
	gchar *appver;

	appver = g_strdup_printf ("%s %s", APPNAME, VERSION);
	markup = g_markup_printf_escaped ("<span size=\"large\"weight=\"bold\">%s</span>", appver);
	gtk_label_set_markup (GTK_LABEL(song_label), markup);
	gtk_label_set_text (GTK_LABEL(artist_label), "http://priyank.one09.net/gimmix");
	gtk_label_set_text (GTK_LABEL(album_label), NULL);
	g_free (markup);
	g_free (appver);
}
