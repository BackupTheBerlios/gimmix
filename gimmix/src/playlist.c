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

GtkTreeModel * 
gimmix_create_and_fill_artists_model (void)
{
	GtkListStore 	*store;
	GtkTreeIter 	iter;
	MpdData 		*data;

	store = gtk_list_store_new (1, G_TYPE_STRING);

	for (data = mpd_database_get_artists(pub->gmo); data != NULL; data = mpd_data_get_next(data))
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, data->tag, -1);
	}
	
	return GTK_TREE_MODEL (store);
}

void
gimmix_update_artists_view_and_model (void)
{
	GtkCellRenderer 	*artists_renderer;
	GtkListStore 		*artists_store;
	GtkTreeModel 		*artists_model;
	GtkTreeSelection	*selection;
	
	artists_store 	= gtk_list_store_new (1, G_TYPE_STRING);
	artists_renderer = gtk_cell_renderer_text_new ();
	
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (directory_treeview),
							-1,
							"Artist",
							artists_renderer,
							"text", 0,
							NULL);
	
	artists_model = gimmix_create_and_fill_artists_model ();

	gtk_tree_view_set_model (GTK_TREE_VIEW (directory_treeview), artists_model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(directory_treeview));
	g_signal_connect(selection, "changed", G_CALLBACK(on_artist_treeview_select), NULL);
	g_object_unref (artists_model);
	return;
}

int
on_artist_treeview_select (void)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *s;
	gchar *artist;
	MpdData *data;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(directory_treeview));
	s = gtk_tree_view_get_selection(GTK_TREE_VIEW(directory_treeview));

	if (gtk_tree_selection_get_selected(s, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &artist, -1);
		g_print(artist);
		gimmix_update_songs_view_and_model (artist);
		return 0;
	}
	else
	{
		return 1;
	}
}

GtkTreeModel *
gimmix_create_and_fill_songs_model (gchar *artist)
{
	GtkListStore 	*store;
	GtkTreeIter 	iter;
	MpdData 		*data;

	store = gtk_list_store_new (1, G_TYPE_STRING);

	for (data = mpd_database_get_albums(pub->gmo, artist); data != NULL; data = mpd_data_get_next(data))
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, data->tag, -1);
	}

	return GTK_TREE_MODEL (store);
}

void
gimmix_update_songs_view_and_model (gchar *artist)
{
	GtkCellRenderer 	*songs_renderer;
	GtkListStore 		*songs_store;
	GtkTreeModel 		*songs_model;
	GtkTreeSelection	*selection;
	
	songs_store 	= gtk_list_store_new (1, G_TYPE_STRING);
	songs_renderer 	= gtk_cell_renderer_text_new ();
	
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (songs_treeview),
							-1,
							"Albums",
							songs_renderer,
							"text", 0,
							NULL);
	
	songs_model = gimmix_create_and_fill_songs_model (artist);

	gtk_tree_view_set_model (GTK_TREE_VIEW (songs_treeview), songs_model);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(songs_treeview));
	//g_signal_connect(selection, "changed", G_CALLBACK(on_artist_treeview_select), NULL);
	g_object_unref (songs_model);
	return;
}

void gimmix_playlist_populate (void)
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
	
	dir_store 	= gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	song_store	= gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	
	song_pixbuf 	= gtk_widget_render_icon (songs_treeview, GTK_STOCK_FILE, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	dir_pixbuf 	= gtk_widget_render_icon (directory_treeview, GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	
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
			gtk_list_store_set (song_store, &song_iter, 0, song_pixbuf, 1, title, -1);
		}
	}

	dir_model	= GTK_TREE_MODEL (dir_store);
	song_model	= GTK_TREE_MODEL (song_store);

	gtk_tree_view_set_model (GTK_TREE_VIEW (directory_treeview), dir_model);
	gtk_tree_view_set_model (GTK_TREE_VIEW (songs_treeview), song_model);
	
	dir_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(directory_treeview));
	g_signal_connect(dir_selection, "changed", G_CALLBACK(on_dir_selected), NULL);
	g_object_unref (dir_model);
	g_object_unref (song_model);
}

void on_dir_selected (void)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *s;
	gchar *artist;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (directory_treeview));
	s = gtk_tree_view_get_selection (GTK_TREE_VIEW (directory_treeview));

	if (gtk_tree_selection_get_selected(s, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 1, &artist, -1);
		g_print(artist);
		//gimmix_update_songs_view_and_model (artist);
	}
	return;
}
void
gimmix_playlist_show (void)
{
	//gimmix_update_artists_view_and_model ();
	gimmix_playlist_populate ();
	//gtk_widget_show_all (playlist_window);
}

