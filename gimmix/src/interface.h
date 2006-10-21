#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>
#include <libnotify/notify.h>
#include "gimmix-core.h"

/* Main window widgets */
GtkWidget *main_window;
GtkWidget *song_label;
GtkWidget *artist_label;
GtkWidget *album_label;
GtkWidget *button_play;
GtkWidget *button_stop;
GtkWidget *button_prev;
GtkWidget *button_next;
GtkWidget *button_prefs;
GtkWidget *button_info;
GtkWidget *volume_scale;
GtkWidget *progress;
GtkWidget *progressbox;
GtkAdjustment *volume_adj;
GtkStatusIcon *tray_icon;
NotifyNotification *notify;

/* Preferences window widgets */
GtkWidget *pref_window;
GtkWidget *host_entry;
GtkWidget *port_entry;
GtkWidget *password_entry;
GtkWidget *systray_toggle;
GtkWidget *button_apply;

/* Info dialog widgets */
GtkWidget *info_window;
GtkWidget *info_file;
GtkWidget *info_title;
GtkWidget *info_artist;
GtkWidget *info_album;
GtkWidget *info_genre;
GtkWidget *info_length;

/* Initializes default interface signals */
void gimmix_init(void);

/* Timer */
gboolean gimmix_timer(void);

/* Utility functions */
GtkWidget * get_image(const gchar *, GtkIconSize);
NotifyNotification * gimmix_notify_init(GtkStatusIcon *);
void gimmix_set_song_info(void);
void gimmix_show_ver_info(void);
gchar * strip_filename(gchar *);

/* Systray and Popup menu */
void gimmix_systray_icon_create(void);
void gimmix_systray_popup_menu(void);

/* Callbacks */
void gimmix_systray_popup_menu(void);
void on_prev_button_clicked(GtkWidget *widget, gpointer data);
void on_next_button_clicked(GtkWidget *widget, gpointer data);
void on_play_button_clicked(GtkWidget *widget, gpointer data);
void on_stop_button_clicked(GtkWidget *widget, gpointer data);
void on_prefs_button_clicked(GtkWidget *widget, gpointer data);
void on_info_button_clicked(GtkWidget *widget, gpointer data);
void on_volume_scale_changed(GtkWidget *widget, gpointer data);
void on_preferences_apply(GtkWidget *widget, gpointer data);
void gimmix_scroll_volume_slider(GtkWidget *, GdkEventScroll *);
void gimmix_progress_seek(GtkWidget *, GdkEvent *);
void gimmix_about_show(void);
void gimmix_window_visible(void);

/* Preferences dialog callbacks */
void on_systray_checkbox_toggled(GtkWidget *, gpointer);

#endif
