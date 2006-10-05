#include <gtk/gtk.h>
#include <stdio.h>
#include "gimmixcore.h"

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
GtkAdjustment *volume_adj;

/* Utility functions */
GtkWidget * get_image(const gchar *, GtkIconSize);
void gimmix_systray_icon_create(void);
void gimmix_systray_popup_menu(void);
void gimmix_set_song_info(void);

/* Callbacks */
void gimmix_init(void);

void gimmix_systray_popup_menu(void);
void on_prev_button_clicked(GtkWidget *widget, gpointer data);
void on_next_button_clicked(GtkWidget *widget, gpointer data);
void on_play_button_clicked(GtkWidget *widget, gpointer data);
void on_stop_button_clicked(GtkWidget *widget, gpointer data);
void on_prefs_button_clicked(GtkWidget *widget, gpointer data);
void on_info_button_clicked(GtkWidget *widget, gpointer data);
void on_volume_scale_changed(GtkWidget *widget, gpointer data);
