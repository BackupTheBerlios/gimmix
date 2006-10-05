#include "interface.h"
#include "gimmix.h"

#define PLAYING 0
#define PAUSED  1

void gimmix_init()
{
	g_signal_connect (G_OBJECT(button_play), "clicked", G_CALLBACK(on_play_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prev), "clicked", G_CALLBACK(on_prev_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_next), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_stop), "clicked", G_CALLBACK(on_stop_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prefs), "clicked", G_CALLBACK(on_prefs_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_info), "clicked", G_CALLBACK(on_info_button_clicked), NULL);
	g_signal_connect(G_OBJECT(volume_scale), "value_changed", G_CALLBACK(on_volume_scale_changed), NULL);

	volume_adj = gtk_range_get_adjustment(GTK_RANGE(volume_scale));
}

void on_prev_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Prev clicked\n");
}

void on_next_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Next clicked\n");
}

void on_play_button_clicked(GtkWidget *widget, gpointer data)
{
	GtkWidget *image;
	gint state;
	
	state = gimmix_play(pub->gmo);
	if(state == PLAYING)
	{
		image = get_image("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(widget), image);
	}
	else if(state == PAUSED)
	{
		image = get_image("gtk-media-play", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(widget), image);
	}
	gimmix_set_song_info();
}

void on_stop_button_clicked(GtkWidget *widget, gpointer data)
{
	//GtkWidget *image;
	
	gimmix_stop(pub->gmo);
}

void on_prefs_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Prefs clicked\n");
}

void on_info_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Info clicked\n");
}

void on_volume_scale_changed(GtkWidget *widget, gpointer data)
{
	gint value;

	value = gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj));
	gimmix_set_volume(pub->gmo, value);
}

GtkWidget * get_image(const gchar *id, GtkIconSize size)
{
	GtkWidget *image;

	image = gtk_image_new_from_stock (id, size);
	return image;
}

void gimmix_set_song_info()
{
	gchar *song_name;
	gchar *artist_name;
	gchar *album_name;
	SongInfo *si;

	si = gimmix_get_song_info(pub->gmo);
	song_name = si->song_name;
	artist_name = si->artist_name;
	album_name = si->album_name;

	gtk_label_set_text(GTK_LABEL(song_label), song_name);
	gtk_label_set_text(GTK_LABEL(artist_label), artist_name);
	gtk_label_set_text(GTK_LABEL(album_label), album_name);
	g_free(si);
}
