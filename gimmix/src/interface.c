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
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */

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
	g_signal_connect (G_OBJECT(progressbox), "button_press_event", G_CALLBACK(gimmix_progress_seek), NULL); 

	volume_adj = gtk_range_get_adjustment(GTK_RANGE(volume_scale));
	gimmix_systray_icon_create();
	gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj), gimmix_get_volume(pub->gmo));
	//gimmix_set_song_info();
	g_timeout_add(50, gimmix_timer, NULL);
}

gboolean gimmix_timer()
{
	gchar time[15];
	float fraction;
	
	if(gimmix_get_progress_status(pub->gmo, &fraction, time))
	{
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), time);
	}
	else
	{
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.0);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "Stopped");
		//return FALSE;
	}
	return TRUE;
}

void on_prev_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Prev clicked\n");
}

void on_next_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Next clicked\n");
}

void on_play_button_clicked(GtkWidget *widget, gpointer data)
{
	GtkWidget *image;
	gint state;
	
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
}

void on_prefs_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Prefs clicked\n");
}

void on_info_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Info clicked\n");
}

void on_volume_scale_changed(GtkWidget *widget, gpointer data)
{
	gint value;

	value = gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj));
	gimmix_set_volume(pub->gmo, value);
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
	//}
	return;
}

GtkWidget * get_image(const gchar *id, GtkIconSize size)
{
	GtkWidget *image;

	image = gtk_image_new_from_stock (id, size);
	return image;
}

void gimmix_set_song_info()
{
	gchar *song_name;
	gchar *artist_name;
	gchar *album_name;
	gchar *markup;
	SongInfo *si;

	si = gimmix_get_song_info(pub->gmo);
	song_name = si->song_name;
	artist_name = si->artist_name;
	album_name = si->album_name;

	markup = g_markup_printf_escaped ("<span style=\"italic\"><b>%s</b></span>", song_name);
	gtk_label_set_markup(GTK_LABEL(song_label), markup);
	gtk_label_set_text(GTK_LABEL(artist_label), artist_name);
	gtk_label_set_text(GTK_LABEL(album_label), album_name);
	g_free(si);
}

void gimmix_systray_icon_create()
{
	gchar *icon_tooltip = "Gimmix";
	tray_icon = gtk_status_icon_new_from_stock("gtk-cdrom");
	gtk_status_icon_set_tooltip(tray_icon, icon_tooltip);
	g_signal_connect (tray_icon, "popup-menu", G_CALLBACK (gimmix_systray_popup_menu), NULL);
}

void gimmix_systray_popup_menu()
{
	GtkWidget *menu, *menu_item;

	menu = gtk_menu_new();

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
	//g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_about_show), NULL);
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
	//g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_prev), NULL);
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
