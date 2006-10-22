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
gimmix_create_and_fill_albums_model (void)
{
	GtkListStore 	*store;
	GtkTreeIter 	iter;
	MpdData 		*data;

	store = gtk_list_store_new (1, G_TYPE_STRING);

	for (data = mpd_database_get_albums(pub->gmo, NULL); data != NULL; data = mpd_data_get_next(data))
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, data->tag, -1);
	}
	
	return GTK_TREE_MODEL (store);
}

void
gimmix_create_view_and_model (void)
{
	GtkCellRenderer *renderer;	
	GtkTreeModel 	*model;
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (album_view),
							-1,
							"Album",
							renderer,
							"text", 0,
							NULL);
	model = gimmix_create_and_fill_albums_model ();
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (album_view), model);
	g_object_unref (model);
}

void 
gimmix_playlist_show (void)
{
	gimmix_create_view_and_model();
	gtk_widget_show_all (playlist_window);
}

