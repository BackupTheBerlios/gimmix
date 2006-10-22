#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <gtk/gtk.h>

/* Playlist */
GtkWidget *playlist_window;
GtkTreeView *album_view;
GtkTreeView *list_view;

GtkTreeModel * gimmix_create_and_fill_albums_model (void);
void gimmix_create_view_and_model (void);
void gimmix_playlist_show (void);

#endif
