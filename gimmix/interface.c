#include "interface.h"

void connect_callbacks()
{
	g_signal_connect (G_OBJECT(button_play), "clicked", G_CALLBACK(on_play_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prev), "clicked", G_CALLBACK(on_prev_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_next), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_stop), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_prefs), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
	g_signal_connect (G_OBJECT(button_info), "clicked", G_CALLBACK(on_next_button_clicked), NULL);
}

void on_prev_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}

void on_next_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}

void on_play_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}

void on_stop_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}

void on_prefs_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}

void on_info_button_clicked(GtkWidget *widget, gpointer data)
{
	g_print("Play clicked");
}
