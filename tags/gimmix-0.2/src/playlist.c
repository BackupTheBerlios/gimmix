/*
 * playlist.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * Gimmix is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Gimmix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Gimmix; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmgg@gmail.com>
 */

#include <string.h>
#include "playlist.h"

#define DELETE_KEY 0xffff

enum {
	SONG = 1,
	DIR,
	PLAYLIST
};

GtkWidget			*current_playlist_treeview;
GtkTreeSelection	*current_playlist_selection;

static void		gimmix_search_init (void);
static void		gimmix_library_search (gint, gchar *);
static void		gimmix_file_browser_populate (void);
static void		gimmix_update_dir_song_treeview_with_dir (gchar *);
static void		gimmix_current_playlist_popup_menu (void);
static void		gimmix_library_popup_menu (void);
static gchar*	gimmix_path_get_parent_dir (gchar *);
static void		gimmix_file_browser_add_song (GtkTreeView *);

/* Callbacks */
/* Current playlist callbacks */
static void		cb_remove_button_clicked (GtkWidget *widget, gpointer data);
static void		cb_clear_button_clicked (GtkWidget *widget, gpointer data);
static void		cb_current_playlist_double_click (GtkTreeView *);
static void		cb_current_playlist_right_click (GtkTreeView *treeview, GdkEventButton *event);
static void cb_repeat_menu_toggled (GtkCheckMenuItem *item, gpointer data);
static void cb_shuffle_menu_toggled (GtkCheckMenuItem *item, gpointer data);
static void		cb_current_playlist_delete_press (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void		gimmix_current_playlist_remove_song (void);
static void		gimmix_current_playlist_clear (void);
static void		gimmix_library_update (GtkWidget *widget, gpointer data);
static gboolean	gimmix_update_player_status (gpointer data);

/* File browser callbacks */
static void		cb_file_browser_close_button_clicked (GtkWidget *widget, gpointer data);
static void		cb_file_browser_dir_activated (GtkTreeView *);
static void		cb_library_right_click (GtkTreeView *treeview, GdkEventButton *event);
static void		cb_search_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);

void
gimmix_playlist_init (void)
{
	GtkWidget			*button;
	GtkWidget 			*window;
	GtkTreeModel		*current_playlist_model;
	GtkListStore		*current_playlist_store;
	GtkCellRenderer     *current_playlist_renderer;
	
	current_playlist_treeview = glade_xml_get_widget (xml, "current_playlist_treeview");
		
	current_playlist_renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (current_playlist_treeview),
							-1,
							"Song",
							current_playlist_renderer,
							"text", 0,
							NULL);
	current_playlist_store = gtk_list_store_new (3,
												G_TYPE_STRING, 	/* name */
												G_TYPE_STRING, 	/* path */
												G_TYPE_INT); 	/* id */
	current_playlist_model	= GTK_TREE_MODEL (current_playlist_store);

	gtk_tree_view_set_model (GTK_TREE_VIEW (current_playlist_treeview), current_playlist_model);
	g_signal_connect (current_playlist_treeview, "row-activated", G_CALLBACK(cb_current_playlist_double_click), NULL);
	g_signal_connect (current_playlist_treeview, "button-release-event", G_CALLBACK (cb_current_playlist_right_click), NULL);
	g_signal_connect (current_playlist_treeview, "key_release_event", G_CALLBACK (cb_current_playlist_delete_press), NULL);
	g_object_unref (current_playlist_model);
	
	/* populate the file browser */
	gimmix_file_browser_populate ();
	
	/* Initialize playlist search */
	gimmix_search_init ();
	
	return;
}

static void
gimmix_search_init (void)
{

	GtkWidget 	*search_combo;
	GtkWidget	*search_entry;
	
	search_combo = glade_xml_get_widget (xml, "search_combo");
	search_entry = glade_xml_get_widget (xml, "search_entry");
	
	gtk_combo_box_set_active (GTK_COMBO_BOX(search_combo), 0);
	g_signal_connect (G_OBJECT(search_entry), "key_release_event", G_CALLBACK(cb_search_keypress), NULL);
	
	return;
}

void
gimmix_update_current_playlist (void)
{
	GtkTreeModel		*current_playlist_model;
	GtkListStore		*current_playlist_store;
	GtkTreeIter			current_playlist_iter;
	gint 				new;
	MpdData 			*data;

	new = mpd_playlist_get_playlist_id (pub->gmo);
	data = mpd_playlist_get_changes (pub->gmo, 0);

	current_playlist_model = gtk_tree_view_get_model (GTK_TREE_VIEW(current_playlist_treeview));
	current_playlist_store = GTK_LIST_STORE (current_playlist_model);
	gtk_list_store_clear (current_playlist_store);

	while (data != NULL)
	{
		gchar 	title[256];
		
		if (data->song->title && data->song->artist)
		{
			snprintf (title, 255, "%s - %s", data->song->artist, data->song->title);
		}
		else
		{
			snprintf (title, 255, "%s", data->song->file);
		}
		
		gtk_list_store_append (current_playlist_store, &current_playlist_iter);
		gtk_list_store_set (current_playlist_store, 
							&current_playlist_iter,
							0, title,
							1, data->song->file,
							2, data->song->id,
							-1);
		data = mpd_data_get_next (data);
	}
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (current_playlist_treeview), current_playlist_model);
	
	mpd_data_free (data);
	
	return;
}

static void
gimmix_file_browser_populate (void)
{
	GtkWidget			*directory_treeview;
	GtkTreeModel 		*dir_model;
	GtkTreeIter  		dir_iter;
	GtkCellRenderer 	*dir_renderer;
	GtkListStore 		*dir_store;
	MpdData 			*data = NULL;
	GdkPixbuf 			*dir_pixbuf;
	GdkPixbuf			*song_pixbuf;
	gchar				*path;
	
	directory_treeview = glade_xml_get_widget (xml, "album");
	
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
	dir_store 	= gtk_list_store_new (4, 
									GDK_TYPE_PIXBUF, 	/* icon */
									G_TYPE_STRING, 		/* name */
									G_TYPE_STRING,		/* path */
									G_TYPE_INT);		/* type DIR/SONG */
	
	
	path = g_strdup_printf ("%s%s", PREFIX, "/share/pixmaps/gimmix.png");
	song_pixbuf = gdk_pixbuf_new_from_file_at_size (path, 12, 12, NULL);
	g_free (path);
	dir_pixbuf 	    = gtk_widget_render_icon (GTK_WIDGET(directory_treeview), GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	
	for (data = mpd_database_get_directory(pub->gmo, NULL); data != NULL; data = mpd_data_get_next(data))
	{
		gtk_list_store_append (dir_store, &dir_iter);
		
		if(data->type == MPD_DATA_TYPE_DIRECTORY)
		{
			gtk_list_store_set (dir_store, &dir_iter,
								0, dir_pixbuf,
								1, g_path_get_basename(data->directory),
								2, data->directory,
								3, DIR,
								-1);
		}
		else if (data->type == MPD_DATA_TYPE_SONG)
		{
			gchar *title;

			title = data->song->title ? data->song->title : data->song->file;
			gtk_list_store_set (dir_store, &dir_iter,
								0, song_pixbuf,
								1, title,
								2, data->song->file,
								3, SONG,
								-1);
		}
	}

	mpd_data_free (data);

	dir_model	= GTK_TREE_MODEL (dir_store);

	gtk_tree_view_set_model (GTK_TREE_VIEW (directory_treeview), dir_model);
	g_signal_connect (directory_treeview, "row-activated", G_CALLBACK(cb_file_browser_dir_activated), NULL);
	g_signal_connect (directory_treeview, "button-release-event", G_CALLBACK(cb_library_right_click), NULL);
	g_object_unref (dir_model);
	g_object_unref (dir_pixbuf);
	g_object_unref (song_pixbuf);
	
	return;
}

static void
gimmix_library_search (gint type, gchar *text)
{
	if (!text)
		return;

	MpdData 		*data;
	GtkWidget		*directory_treeview;
	GtkTreeModel	*directory_model;
	GtkListStore	*dir_store;
	GdkPixbuf 		*song_pixbuf;
	GtkTreeIter 	dir_iter;
	gchar 			*path;

	switch (type)
	{
		/* Search by Artist */
		case 0: data = mpd_database_find (pub->gmo, MPD_TABLE_ARTIST, text, FALSE);
				break;
		
		/* Search by Album */
		case 1: data = mpd_database_find (pub->gmo, MPD_TABLE_ALBUM, text, FALSE);
				break;
		
		/* Search by Title */
		case 2: data = mpd_database_find (pub->gmo, MPD_TABLE_TITLE, text, FALSE);
				break;
		
		/* Search by File name */
		case 3: data = mpd_database_find (pub->gmo, MPD_TABLE_FILENAME, text, FALSE);
				break;
				
		default: return;
	}
	
	directory_treeview = glade_xml_get_widget (xml, "album");
	directory_model = gtk_tree_view_get_model (GTK_TREE_VIEW (directory_treeview));
	dir_store 	= GTK_LIST_STORE (directory_model);

	/* Clear the stores */
	gtk_list_store_clear (dir_store);
	
	if (!data)
	{
		GdkPixbuf *icon	= gtk_widget_render_icon (GTK_WIDGET(directory_treeview), 												"gtk-dialog-error",
											GTK_ICON_SIZE_MENU,
											NULL);
		gtk_list_store_append (dir_store, &dir_iter);
		gtk_list_store_set (dir_store, &dir_iter,
								0, icon,
								1, "No Result.",
								2, NULL,
								3, 0,
								-1);
		directory_model = GTK_TREE_MODEL (dir_store);
		gtk_tree_view_set_model (GTK_TREE_VIEW(directory_treeview), directory_model);
		g_object_unref (icon);
		return;
	}


	path = g_strdup_printf ("%s%s", PREFIX, "/share/pixmaps/gimmix.png");
	song_pixbuf = gdk_pixbuf_new_from_file_at_size (path, 12, 12, NULL);
	g_free (path);

	for (data; data!=NULL; data = mpd_data_get_next (data))
	{
		if (data->type == MPD_DATA_TYPE_SONG)
		{
			gchar *title;
			
			title = data->song->title ? data->song->title : g_path_get_basename(data->song->file);
			gtk_list_store_append (dir_store, &dir_iter);
			gtk_list_store_set (dir_store, &dir_iter,
								0, song_pixbuf,
								1, title,
								2, data->song->file,
								3, SONG,
								-1);
		}
	}
	
	mpd_data_free (data);

	directory_model = GTK_TREE_MODEL (dir_store);
	gtk_tree_view_set_model (GTK_TREE_VIEW(directory_treeview), directory_model);
	g_object_unref (song_pixbuf);
	
	return;
}

static void
cb_search_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	GtkWidget	*entry;
	GtkWidget	*combo;
	gint		index;
	gchar		text[20];
	
	entry = glade_xml_get_widget (xml, "search_entry");
	strcpy (text, gtk_entry_get_text (GTK_ENTRY(entry)));
	
	if ( (strlen (text)) <= 1 )
	{
		gimmix_update_dir_song_treeview_with_dir ("/");
		return;
	}
	
	combo = glade_xml_get_widget (xml, "search_combo");
	index = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));

	gimmix_library_search (index, text);
}

static void
cb_current_playlist_delete_press (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != DELETE_KEY)
		return;
	
	gimmix_current_playlist_remove_song ();
	
	return;
}

static void
cb_remove_button_clicked (GtkWidget *widget, gpointer data)
{
	gimmix_current_playlist_remove_song ();
	
	return;
}

static void
cb_clear_button_clicked (GtkWidget *widget, gpointer data)
{
	gimmix_current_playlist_clear ();
	
	return;
}

static void
cb_file_browser_close_button_clicked (GtkWidget *widget, gpointer data)
{
	gtk_widget_hide (GTK_WIDGET(data));
	
	return;
}

static void
cb_current_playlist_double_click (GtkTreeView *treeview)
{
	GtkTreeModel 		*model;
	GtkTreeSelection 	*selected;
	GtkTreeIter			iter;

	gchar 				*path;
	gchar 				*title;
	gint 				id;

	selected = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));

	if ( gtk_tree_selection_get_selected (selected, &model, &iter) )
	{
		gtk_tree_model_get (model, &iter,
							0, &title,
							1, &path,
							2, &id,
							-1);
		mpd_player_play_id (pub->gmo, id);
		mpd_status_update (pub->gmo);
		gimmix_set_song_info ();
	}
	
	return;
}

static void
gimmix_file_browser_add_song (GtkTreeView *treeview)
{
	GtkTreeSelection 	*selected;
	GtkTreeIter			current_playlist_tree_iter;
	GtkTreeModel 		*model;
	GtkTreeIter			iter;
	gchar 				*path;
	gchar 				*title;
	gint				id;
	MpdData				*data;
	
	selected = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
									
	if (gtk_tree_selection_get_selected(selected, &model, &iter))
	{
		gtk_tree_model_get (model, &iter,
							1, &title,
							2, &path,
							3, &id,
							-1);
		mpd_playlist_add (pub->gmo, path);
		data = mpd_playlist_get_changes (pub->gmo, mpd_playlist_get_playlist_id(pub->gmo));
		mpd_status_update (pub->gmo);
		mpd_data_free (data);
		
		gimmix_update_current_playlist ();
	}

	return;
}

static void
cb_file_browser_dir_activated (GtkTreeView *treeview)
{
	GtkTreeModel 		*model;
	GtkTreeSelection 	*selection;
	GtkTreeIter 		iter;
	gchar 				*dir;
	gint				type;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if ( gtk_tree_selection_get_selected (selection, &model, &iter) )
		gtk_tree_model_get (model, &iter, 2, &dir, 3, &type, -1);
	
	if (type == DIR)
		gimmix_update_dir_song_treeview_with_dir (dir);
	else if (type == SONG)
		gimmix_file_browser_add_song (treeview);

	return;
}

static void
gimmix_update_dir_song_treeview_with_dir (gchar *dir)
{
	GtkWidget			*directory_treeview;
	GtkTreeModel		*directory_model;
	GtkListStore		*dir_store;
	GtkTreeIter			dir_iter;
	GdkPixbuf			*dir_pixbuf;
	GdkPixbuf			*song_pixbuf;
	MpdData				*data;
	gchar				*parent;
	gchar				*path;
	
	directory_treeview = glade_xml_get_widget (xml, "album");
	directory_model = gtk_tree_view_get_model (GTK_TREE_VIEW (directory_treeview));
	dir_store 	= GTK_LIST_STORE (directory_model);

	if (!strlen(dir))
		dir = "/";

	/* Clear the stores */
	gtk_list_store_clear (dir_store);

	dir_pixbuf 	= gtk_widget_render_icon (GTK_WIDGET(directory_treeview), 											GTK_STOCK_DIRECTORY,
										GTK_ICON_SIZE_SMALL_TOOLBAR,
										NULL);
	path = g_strdup_printf ("%s%s", PREFIX, "/share/pixmaps/gimmix.png");
	song_pixbuf = gdk_pixbuf_new_from_file_at_size (path, 12, 12, NULL);
	g_free (path);
	
	if (dir != "/")
	{	
		parent = gimmix_path_get_parent_dir (dir);
		gtk_list_store_append (dir_store, &dir_iter);
		gtk_list_store_set (dir_store, &dir_iter,
								0, dir_pixbuf,
								1, "..",
								2, parent,
								3, DIR,
								-1);
	}
	
	for (data = mpd_database_get_directory(pub->gmo, dir); data != NULL; data = mpd_data_get_next(data))
	{
		gtk_list_store_append (dir_store, &dir_iter);
		
		if (data->type == MPD_DATA_TYPE_DIRECTORY)
		{
			gtk_list_store_set (dir_store, &dir_iter,
								0, dir_pixbuf,
								1, g_path_get_basename(data->directory),
								2, data->directory,
								3, DIR,
								-1);
		}
		else if (data->type == MPD_DATA_TYPE_SONG)
		{
			gchar *title;
			
			title = data->song->title ? data->song->title : g_path_get_basename(data->song->file);
			gtk_list_store_set (dir_store, &dir_iter,
								0, song_pixbuf,
								1, title,
								2, data->song->file,
								3, SONG,
								-1);
		}
	}
	
	mpd_data_free (data);

	directory_model = GTK_TREE_MODEL (dir_store);
	gtk_tree_view_set_model (GTK_TREE_VIEW(directory_treeview), directory_model);
	
	g_object_unref (dir_pixbuf);
	g_object_unref (song_pixbuf);
	
	return;
}

static void
cb_current_playlist_right_click (GtkTreeView *treeview, GdkEventButton *event)
{	
	if (event->button == 3) /* If right click */
	{
		gimmix_current_playlist_popup_menu ();
	}
	return;
}

static void
cb_library_right_click (GtkTreeView *treeview, GdkEventButton *event)
{	
	if (event->button == 3) /* If right click */
	{
		gimmix_library_popup_menu ();
	}
	return;
}

static void
gimmix_current_playlist_remove_song (void)
{
	GtkTreeModel		*current_playlist_model;
	GtkTreeSelection	*selection;
	GtkTreeIter			iter;
	gchar				*title;
	gchar				*path;
	gint				id;

	current_playlist_model = gtk_tree_view_get_model (GTK_TREE_VIEW(current_playlist_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(current_playlist_treeview));
	
	if ( gtk_tree_selection_get_selected (selection, &current_playlist_model, &iter) )
	{
		gtk_tree_model_get (current_playlist_model, &iter,
							0, &title,
							1, &path,
							2, &id,
							-1);
		mpd_playlist_delete_id (pub->gmo, id);
		mpd_status_update (pub->gmo);
		gtk_list_store_remove (GTK_LIST_STORE (current_playlist_model), &iter);	
	}
}	

static void
gimmix_current_playlist_clear (void)
{
	GtkListStore	*current_playlist_store;
	
	current_playlist_store = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(current_playlist_treeview)));
	gtk_list_store_clear (GTK_LIST_STORE(current_playlist_store));
	mpd_playlist_clear (pub->gmo);
	mpd_status_update (pub->gmo);

	return;
}

static void
gimmix_current_playlist_popup_menu (void)
{
	GtkWidget *menu, *menu_item;

	menu = gtk_menu_new ();
	
	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_REMOVE, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_current_playlist_remove_song), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLEAR, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_current_playlist_clear), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	menu_item = gtk_check_menu_item_new_with_label ("Repeat");
	g_signal_connect (G_OBJECT(menu_item), "toggled", G_CALLBACK(cb_repeat_menu_toggled), NULL);
	if (is_gimmix_repeat(pub->gmo))
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu_item), TRUE);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	menu_item = gtk_check_menu_item_new_with_label ("Shuffle");
	g_signal_connect (G_OBJECT(menu_item), "toggled", G_CALLBACK(cb_shuffle_menu_toggled), NULL);
	if (is_gimmix_shuffle(pub->gmo))
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu_item), TRUE);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	gtk_widget_show (menu);
	gtk_menu_popup (GTK_MENU(menu),
					NULL,
					NULL,
					NULL,
					NULL,
					3,
					gtk_get_current_event_time());
}

static void
cb_repeat_menu_toggled (GtkCheckMenuItem *item, gpointer data)
{
	gboolean state;
	
	state = gtk_check_menu_item_get_active (item);
	if (state == TRUE)
	{
		gimmix_repeat (pub->gmo, true);
	}
	else if (state == FALSE)
	{
		gimmix_repeat (pub->gmo, false);
	}
	
	return;
}

static void
cb_shuffle_menu_toggled (GtkCheckMenuItem *item, gpointer data)
{
	gboolean state;
	
	state = gtk_check_menu_item_get_active (item);
	if (state == TRUE)
	{
		gimmix_shuffle (pub->gmo, true);
	}
	else if (state == FALSE)
	{
		gimmix_shuffle (pub->gmo, false);
	}
	
	return;
}

static void
gimmix_library_update (GtkWidget *widget, gpointer data)
{
	GtkWidget *label;
	
	label = glade_xml_get_widget (xml, "gimmix_status");
	mpd_database_update_dir (pub->gmo, "/");
	gtk_label_set_text (GTK_LABEL(label), "Updating Library...");
	gtk_widget_show (label);
	g_timeout_add (300, (GSourceFunc)gimmix_update_player_status, label);
	
	return;
}

static gboolean
gimmix_update_player_status (gpointer data)
{
	if (mpd_status_db_is_updating (pub->gmo))
		return TRUE;
	else
	{
		gtk_widget_hide (GTK_WIDGET(data));
		gimmix_update_dir_song_treeview_with_dir ("/");
		return FALSE;
	}
	
	return FALSE;
}

static void
gimmix_library_popup_menu (void)
{
	GtkWidget 			*menu;
	GtkWidget 			*menu_item;
	GtkWidget			*image;

	menu = gtk_menu_new ();
	image = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_MENU);
	
	menu_item = gtk_image_menu_item_new_with_label ("Update Library");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_library_update), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	gtk_widget_show (menu);
	gtk_menu_popup (GTK_MENU(menu),
					NULL,
					NULL,
					NULL,
					NULL,
					3,
					gtk_get_current_event_time());
}

static gchar *
gimmix_path_get_parent_dir (gchar *path)
{
	gchar *p, buf[128];
	gchar *ret;

	strcpy (buf, path);
	p = strrchr (buf, '/');
	if (p)
	{	
		*p = 0;
		ret = g_strdup (buf);
 		return ret;
 	}
 	else
 	return "/";
}
