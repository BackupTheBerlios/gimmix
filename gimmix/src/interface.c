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
#include "interface.h"
#include "gimmix.h"

#define PLAYING 0
#define PAUSED  1

void gimmix_init()
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

	volume_adj = gtk_range_get_adjustment(GTK_RANGE(volume_scale));
	if(pub->conf->systray_enable == 1)
		gimmix_systray_icon_create();
	gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj), gimmix_get_volume(pub->gmo));

	if(gimmix_is_playing(pub->gmo))
	{
		gimmix_set_song_info();
		GtkWidget *image = get_image("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(button_play), image);
	}
	gimmix_show_ver_info();
	g_timeout_add(50, gimmix_timer, NULL);
}

gboolean gimmix_timer()
{
	gchar time[15];
	float fraction;
	
	if(gimmix_is_playing(pub->gmo))
	{
		gimmix_get_progress_status(pub->gmo, &fraction, time);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), time);
		return TRUE;
	}
	GtkWidget *image = get_image("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button_play), image);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "Stopped");
	return FALSE;
}

void on_prev_button_clicked(GtkWidget *widget, gpointer data)
{
	if(gimmix_prev(pub->gmo))
		gimmix_set_song_info();
}

void on_next_button_clicked(GtkWidget *widget, gpointer data)
{
	if(gimmix_next(pub->gmo))
		gimmix_set_song_info();
}

void on_play_button_clicked(GtkWidget *widget, gpointer data)
{
	GtkWidget *image;
	gint state;
	
	if(!gimmix_is_playing(pub->gmo))
	{
		g_timeout_add(100, gimmix_timer, NULL);		
	}
	state = gimmix_play(pub->gmo);
	if(state == PLAYING)
	{
		image = get_image("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(button_play), image);		
	}
	else if(state == PAUSED)
	{
		image = get_image("gtk-media-play", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(button_play), image);
	}
	else
		return;
	gimmix_set_song_info();
}

void on_stop_button_clicked(GtkWidget *widget, gpointer data)
{
	GtkWidget *image;
	
	gimmix_stop(pub->gmo);
	image = get_image("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button_play), image);
	gimmix_show_ver_info();
}

void on_prefs_button_clicked(GtkWidget *widget, gpointer data)
{
	Conf *conf;
	gchar port[8];
	gint systray_enable;

	conf = pub->conf;
	sprintf(port, "%d", conf->port);
	systray_enable = conf->systray_enable;

	gtk_entry_set_text(GTK_ENTRY(host_entry), conf->hostname);
	gtk_entry_set_text(GTK_ENTRY(port_entry), port);
	if(conf->password)
		gtk_entry_set_text(GTK_ENTRY(password_entry), conf->password);
	if(systray_enable == 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(systray_toggle), TRUE);
	g_signal_connect (G_OBJECT(systray_toggle), "toggled", G_CALLBACK(on_systray_checkbox_toggled), NULL);
	gtk_widget_show(GTK_WIDGET(pref_window));
}

void on_info_button_clicked(GtkWidget *widget, gpointer data)
{
	if(gimmix_is_playing(pub->gmo))
	{
		SongInfo *info = NULL;
		gchar *length;
		info = gimmix_get_song_info(pub->gmo);

		if(info->file)
			gtk_entry_set_text(GTK_ENTRY(info_file), info->file);
		if(info->title)
			gtk_label_set_text(GTK_LABEL(info_title), info->title);
		if(info->artist)
			gtk_label_set_text(GTK_LABEL(info_artist), info->artist);
		if(info->album)
			gtk_label_set_text(GTK_LABEL(info_album), info->album);
		length = gimmix_get_song_length(info);
		if(length)
		{	
			gtk_label_set_text(GTK_LABEL(info_length), length);
			g_free(length);
		}
		if(info->genre)
			gtk_label_set_text(GTK_LABEL(info_genre), info->genre);
		gimmix_free_song_info(info);

		gtk_widget_show(GTK_WIDGET(info_window));
	}
}

void on_volume_scale_changed(GtkWidget *widget, gpointer data)
{
	gint value;

	value = gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj));
	gimmix_set_volume(pub->gmo, value);
}

void gimmix_scroll_volume_slider(GtkWidget *object, GdkEventScroll *event)
{
	if(event->type != GDK_SCROLL)
		return;

	gint volume;
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

void gimmix_progress_seek(GtkWidget *progressbox, GdkEvent *event)
{
	GtkAllocation allocation;
	gint x, newtime, totaltime;
	gdouble seektime;

	x = event->button.x;
	allocation = GTK_WIDGET(progressbox)->allocation;
	totaltime = gimmix_get_total_song_time(pub->gmo);
	seektime = (gdouble)x/allocation.width;
	newtime = seektime * totaltime;
	if(gimmix_seek(pub->gmo, newtime))
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), seektime);
	return;
}

void gimmix_window_visible()
{
	if(!GTK_WIDGET_VISIBLE(main_window))
		gtk_widget_show(GTK_WIDGET(main_window));
	else
		gtk_widget_hide(GTK_WIDGET(main_window));
}

GtkWidget * get_image(const gchar *id, GtkIconSize size)
{
	GtkWidget *image;

	image = gtk_image_new_from_stock (id, size);
	return image;
}

void gimmix_set_song_info()
{
	gchar *markup;
	SongInfo *song = NULL;
	
	song = gimmix_get_song_info(pub->gmo);

	if(song->title)
	{
		markup = g_markup_printf_escaped("<span size=\"medium\"weight=\"bold\"><i>%s</i></span>", song->title);
		gtk_label_set_markup(GTK_LABEL(song_label), markup);
	}
	if(song->artist)
		gtk_label_set_text(GTK_LABEL(artist_label), song->artist);
	if(song->album)
		gtk_label_set_text(GTK_LABEL(album_label), song->album);

	gimmix_free_song_info(song);
}

void gimmix_systray_icon_create()
{
	gchar *icon_tooltip = "Gimmix";
	tray_icon = gtk_status_icon_new_from_stock("gtk-cdrom");
	gtk_status_icon_set_tooltip(tray_icon, icon_tooltip);
	g_signal_connect (tray_icon, "popup-menu", G_CALLBACK (gimmix_systray_popup_menu), NULL);
	g_signal_connect (tray_icon, "activate", G_CALLBACK(gimmix_window_visible), NULL);
}

void gimmix_systray_popup_menu()
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
	//g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_next), NULL);
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
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,gtk_get_current_event_time());
}

/* Callbacks for preferences dialog */
void on_systray_checkbox_toggled(GtkWidget *widget, gpointer data)
{
	g_object_unref(tray_icon);
}

void gimmix_about_show(void)
{
	GdkPixbuf *about_pixbuf;
	about_pixbuf= gdk_pixbuf_new_from_file("gimmix.png", NULL);

	gchar *website = "http://priyank.one09.net/";
	gchar *website_label = "http://priyank.one09.net/";
	gchar *authors[] = 	{ "Priyank M. Gosalia <priyankmg@gmail.com>",
						 "Part of song seek code taken from Pygmy.",
						 NULL
						};
	
	gtk_show_about_dialog (NULL,
                           "name", "Gimmix",
                           "version", "0.1alpha",
                           "copyright", "\xC2\xA9 2006 Priyank Gosalia  (GPL)",
                           "comments", "Gimmix is a GUI MPD client written in C",
                           "authors", authors,
                           "website", website,
                           "website-label", website_label,
                           "logo", about_pixbuf, 
                           NULL);
	return;
}

void gimmix_show_ver_info()
{
	gchar *markup;

	markup = g_markup_printf_escaped("<span size=\"large\"weight=\"bold\">%s</span>", "Gimmix 0.1 alpha");
	gtk_label_set_markup(GTK_LABEL(song_label), markup);
	gtk_label_set_text(GTK_LABEL(artist_label), "http://priyank.one09.net/gimmix");
	gtk_label_set_text(GTK_LABEL(album_label), NULL);
	free(markup);
}
