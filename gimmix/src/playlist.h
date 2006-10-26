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
GtkTreeView *current_playlist_treeview;
GtkTreeView *directory_treeview;
GtkTreeView *songs_treeview;

void gimmix_current_playlist_init (void);
void gimmix_update_current_playlist (void);
void gimmix_playlist_populate (void);
void gimmix_update_dir_song_treeview_with_dir (gchar *);
gchar *gimmix_path_get_parent_dir (gchar *);

/* Callbacks */
void add_song (GtkTreeView *);
void on_dir_activated (GtkTreeView *);
void gimmix_current_playlist_play (GtkTreeView *);

#endif
