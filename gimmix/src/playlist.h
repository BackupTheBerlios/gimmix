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


/* Callbacks */
void on_dir_selected (void);
void add_song (GtkTreeView *);
void gimmix_current_playlist_play (GtkTreeView *);
#endif
