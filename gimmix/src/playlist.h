#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "gimmix-core.h"
#include "gimmix.h"
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include <gtk/gtk.h>

/* Playlist */
GtkWidget *playlist_window;
GtkTreeView *artists_treeview;
GtkTreeView *songs_treeview;

GtkTreeModel * gimmix_update_and_fill_artists_model (void);
void gimmix_update_artists_view_and_model (void);

GtkTreeModel * gimmix_update_and_fill_songs_model (gchar *);
void gimmix_update_songs_view_and_model (gchar *);

void gimmix_playlist_show (void);

/* Callbacks */
int on_artist_treeview_select (void);

#endif
