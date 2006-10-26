/*
 * playlist.c
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

#include "playlist.h"

enum {
	SONG = 1,
	DIR,
	PLAYLIST
};

GtkTreeModel        *current_playlist_model;
GtkListStore        *current_playlist_store;
GtkCellRenderer     *current_playlist_renderer;

void
gimmix_current_playlist_init (void)
{
	current_playlist_renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (current_playlist_treeview),
							-1,
							"Song",
							current_playlist_renderer,
							"text", 0,
							NULL);
	current_playlist_store = gtk_list_store_new (2,
												G_TYPE_STRING, /* name */
												G_TYPE_STRING); /* path */
	current_playlist_model	= GTK_TREE_MODEL (current_playlist_store);
	gtk_tree_view_set_model (GTK_TREE_VIEW (current_playlist_treeview), current_playlist_model);
	//g_object_unref (current_playlist_tree_model);
}

void
gimmix_update_current_playlist (void)
{
	GtkTreeIter			current_playlist_iter;
	GtkTreeSelection	*selection;
	MpdData 			*data;
	gint 				new;
	
	new = mpd_playlist_get_playlist_id (pub->gmo);
	g_printf("\n === Current Playlist ID is %d === \n", new);
	data = mpd_playlist_get_changes (pub->gmo, 0);
	
	current_playlist_model = gtk_tree_view_get_model (GTK_TREE_VIEW(current_playlist_treeview));
	current_playlist_store = GTK_LIST_STORE (current_playlist_model);
	
	g_print ("\n === Getting playlist contents === \n");
	while (data != NULL)
	{
		gchar *title;
		
		title = data->song->title ? data->song->title : data->song->file;
		g_print (title);
		g_print ("\n");
		gtk_list_store_append (current_playlist_store, &current_playlist_iter);
		gtk_list_store_set (current_playlist_store, &current_playlist_iter, 0, title, -1);
		data = mpd_data_get_next (data);
	}
	g_print (" === End === \n");
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (current_playlist_treeview), current_playlist_model);
	//selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(songs_treeview));
	//g_signal_connect(selection, "changed", G_CALLBACK(on_artist_treeview_select), NULL);
	return;
}

void
gimmix_playlist_populate (void)
{
	GtkTreeModel 		*dir_model;
	GtkTreeModel		*song_model;
	GtkTreeIter  		dir_iter;
	GtkTreeIter			song_iter;
	GtkCellRenderer 	*dir_renderer;
	GtkCellRenderer		*song_renderer;
	GtkListStore 		*dir_store;
	GtkListStore		*song_store;
	MpdData 			*data = NULL;
	GdkPixbuf 			*dir_pixbuf;
	GdkPixbuf			*song_pixbuf;
	GtkTreeSelection	*dir_selection;
	
	dir_renderer		= gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (directory_treeview),
							-1,
							"Icon",
							dir_renderer,
							"pixbuf", 0,
							NULL);
	
	dir_renderer 		= gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (directory_treeview),
							-1,
							"Albums",
							dir_renderer,
							"text", 1,
							NULL);
							
	song_renderer		= gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (songs_treeview),
							-1,
							"Icon",
							song_renderer,
							"pixbuf", 0,
							NULL);
	song_renderer 		= gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (songs_treeview),
							-1,
							"Song",
							song_renderer,
							"text", 1,
							NULL);
	
	dir_store 	= gtk_list_store_new (3, 
									GDK_TYPE_PIXBUF, /* icon */
									G_TYPE_STRING, /* name */
									G_TYPE_STRING); /* path */
	song_store	= gtk_list_store_new (3, 
									GDK_TYPE_PIXBUF, /* icon */
									G_TYPE_STRING, /* name */
									G_TYPE_STRING); /* path */
	
	song_pixbuf 	= gtk_widget_render_icon (GTK_WIDGET(songs_treeview), GTK_STOCK_FILE, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	dir_pixbuf 	    = gtk_widget_render_icon (GTK_WIDGET(directory_treeview), GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	
	for (data = mpd_database_get_directory(pub->gmo, NULL); data != NULL; data = mpd_data_get_next(data))
	{
		if(data->type == MPD_DATA_TYPE_DIRECTORY)
		{
			gtk_list_store_append (dir_store, &dir_iter);
			gtk_list_store_set (dir_store, &dir_iter, 0, dir_pixbuf, 1, data->directory, -1);
		}
		else if (data->type == MPD_DATA_TYPE_SONG)
		{
			gchar *title;
			gtk_list_store_append (song_store, &song_iter);
			title = data->song->title ? data->song->title : data->song->file;
			gtk_list_store_set (song_store, &song_iter, 0, song_pixbuf, 1, title, 2, data->song->file, -1);
		}
	}

	dir_model	= GTK_TREE_MODEL (dir_store);
	song_model	= GTK_TREE_MODEL (song_store);

	gtk_tree_view_set_model (GTK_TREE_VIEW (directory_treeview), dir_model);
	gtk_tree_view_set_model (GTK_TREE_VIEW (songs_treeview), song_model);
	g_signal_connect (songs_treeview, "row-activated", G_CALLBACK(add_song), NULL);
	
	dir_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(directory_treeview));
	g_signal_connect(dir_selection, "changed", G_CALLBACK(on_dir_selected), NULL);
	g_object_unref (dir_model);
	g_object_unref (song_model);
}

void
add_song (GtkTreeView *treeview)
{
	GtkTreeSelection 	*selected;
	GtkTreeIter			current_playlist_tree_iter;
	GtkTreeModel 		*model;
	GtkTreeIter			iter;
	gchar 				*path;
	gchar 				*title;
	
	selected = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
									
	if (gtk_tree_selection_get_selected(selected, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 1, &title, 2, &path, -1);
		mpd_playlist_add (pub->gmo, path);
		gtk_list_store_append (current_playlist_store, &current_playlist_tree_iter);
		gtk_list_store_set (current_playlist_store, &current_playlist_tree_iter, 0, title, 1, path, -1);
	}

	current_playlist_model	= GTK_TREE_MODEL (current_playlist_store);
	gtk_tree_view_set_model (GTK_TREE_VIEW (current_playlist_treeview), current_playlist_model);
	//g_object_unref (current_playlist_tree_model);
}

void
on_dir_selected (void)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *s;
	gchar *artist;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (directory_treeview));
	s = gtk_tree_view_get_selection (GTK_TREE_VIEW (directory_treeview));

	if (gtk_tree_selection_get_selected(s, &model, &iter))
		gtk_tree_model_get(model, &iter, 1, &artist, -1);
	
	return;
}

