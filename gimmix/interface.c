#include "interface.h"

void connect_callbacks()
{
	g_signal_connect (G_OBJECT(button_play), "clicked", G_CALLBACK(on_play_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prev), "clicked", G_CALLBACK(on_prev_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_next), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_stop), "clicked", G_CALLBACK(on_stop_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prefs), "clicked", G_CALLBACK(on_prefs_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_info), "clicked", G_CALLBACK(on_info_button_clicked), NULL);
	g_signal_connect(G_OBJECT(volume_scale), "value_changed", G_CALLBACK(on_volume_scale_changed), NULL);
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
	g_print("Play clicked\n");
}

void on_stop_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Stop clicked\n");
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
	g_print("volume scale updated\n");
}
