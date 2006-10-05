#include <gtk/gtk.h>
#include <glade/glade.h>
#include "gimmix.h"
#include "interface.h"

int gimmix_connect(void)
{
	MpdObj *mo;
	
	if((mo = gimmix_mpd_connect()))
	{	
		pub->gmo = mo;
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	GladeXML *xml;
	GtkWidget *main_window;

	pub = (GM *) malloc(sizeof(GM));
	
	if(gimmix_connect())
	{
		gtk_init(&argc, &argv);

		xml = glade_xml_new("glade/gimmix.glade", NULL, NULL);
		glade_xml_signal_autoconnect(xml);

		main_window = glade_xml_get_widget(xml, "main_window");
		song_label = glade_xml_get_widget(xml,"song_label");
		artist_label = glade_xml_get_widget(xml,"artist_label");
		album_label = glade_xml_get_widget(xml,"album_label");
		button_play = glade_xml_get_widget(xml,"play_button");
		button_prev = glade_xml_get_widget(xml,"prev_button");
		button_next = glade_xml_get_widget(xml,"next_button");
		button_stop = glade_xml_get_widget(xml,"stop_button");
		button_prefs = glade_xml_get_widget(xml,"pref_button");
		button_info = glade_xml_get_widget(xml,"info_button");
		volume_scale = glade_xml_get_widget(xml,"volume_scale");
		progress = glade_xml_get_widget(xml,"progress");
		progressbox = glade_xml_get_widget(xml,"progress_event_box");

		gimmix_init();

		gtk_main();
	}

	return(0);
}
