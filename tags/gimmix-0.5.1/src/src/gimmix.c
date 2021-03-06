/*
 * gimmix.c
 *
 * Copyright (C) 2006-2008 Priyank Gosalia
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
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */

#include <gtk/gtk.h>
#include <locale.h>
#include <getopt.h>
#include <libgen.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "gimmix.h"
#include "gimmix-firstrun.h"
#include "gimmix-interface.h"
#include "gimmix-playlist.h"

#define GLADE_FILE	"/share/gimmix/gimmix.glade"
#define GIMMIX_ICON	"gimmix.png"

MpdObj 		*gmo = NULL;
GladeXML 	*xml = NULL;
GtkWidget	*connection_box = NULL;

static void error_dialog_response (GtkDialog *err_dialog, gint arg1, gpointer dialog);
static void gimmix_mpd_connection_changed (MpdObj *mo, int connect, void *userdata);

bool
gimmix_connect (void)
{
	MpdObj *mo;

	mo = gimmix_mpd_connect ();
	
	if (mo != NULL)
	{
		gmo = mo;
		mpd_signal_connect_connection_changed (gmo, (ConnectionChangedCallback)gimmix_mpd_connection_changed, NULL);
		return true;
	}
	
	gmo = NULL;
	return false;
}

static void
gimmix_mpd_connection_changed (MpdObj *mo, int connect, void *userdata)
{
	if (!connect) /* disconnected */
	{
		g_print ("disconnected from mpd\n");
		//mpd_free (gmo);
		//gmo = NULL;
		gimmix_interface_disable_controls ();
		gtk_widget_show (connection_box);
	}
	else /* connected */
	{
		g_print ("connected to mpd\n");
		gimmix_interface_enable_controls ();
		gtk_widget_hide (connection_box);
		gimmix_init ();
	}
	
	return;
}

void
gimmix_connect_error (void)
{
	GtkWidget	*error_dialog;
	gchar		*error;
	
	error = _("Gimmix couldn't connect to mpd. \n\nCheck whether mpd is running.\nAlso check that you have specified the proper hostname, port and password in ~/.gimmixrc");
	
	error_dialog = gtk_message_dialog_new_with_markup (NULL,
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"<b>%s: </b><span size=\"large\">%s</span>",
							_("ERROR"),
							error);
	g_signal_connect (error_dialog,
			"response",
			G_CALLBACK (error_dialog_response),
			(gpointer)error_dialog);
	
	gtk_widget_show_all (error_dialog);
    
	return;
}

static void
error_dialog_response (GtkDialog *err_dialog, gint arg1, gpointer dialog)
{
	gtk_widget_destroy (GTK_WIDGET(dialog));
	gtk_main_quit ();
	
	return;
}

static void
cb_gimmix_connect_button_clicked (GtkWidget *widget, gpointer data)
{
	if (gimmix_connect())
	{
		gimmix_interface_enable_controls ();
		gimmix_init ();
		gtk_widget_hide (connection_box);
	}
	
	return;
}

void
gimmix_about_show (void)
{
	GdkPixbuf		*about_pixbuf;
	gchar			*path;
	static gchar		*license = 
	("Gimmix is free software; you can redistribute it and/or "
	"modify it under the terms of the GNU General Public Licence as "
	"published by the Free Software Foundation; either version 2 of the "
	"Licence, or (at your option) any later version.\n"
	"\n"
	"Gimmix is distributed in the hope that it will be useful, "
	"but WITHOUT ANY WARRANTY; without even the implied warranty of "
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
	"General Public Licence for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public Licence "
	"along with Gimmix; if not, write to the Free Software "
	"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, "
	"MA  02110-1301  USA");
	
	path = gimmix_get_full_image_path (GIMMIX_ICON);
	about_pixbuf = gdk_pixbuf_new_from_file (path, NULL);
	g_free (path);

	gchar *authors[] = { "Priyank M. Gosalia <priyankmg@gmail.com>",
					"Gimmix uses wejpconfig written by Johannes Heimansberg.",
					_("A part of the song seek code borrowed from Pygmy."),
					"Alex Smith: Autoconf 2.5 support",
				 	"Rohan Dhruva: Extensive testing and bug reports",
					 NULL
					};
	static gchar translators[] = \
				"Deutsch (de) - Martin Stromberger <mstromberger@aon.at>\n"
				"Traditional Chinese (zh_TW) - Cheng-Wei Chien <e.cwchien@gmail.com>\n"
				"Turkish (tr) - Yavuz Selim Burgu <turkalinux@gmail.com>\n";


	gtk_show_about_dialog (NULL,
                           "name", APPNAME,
                           "version", VERSION,
                           "copyright", _("\xC2\xA9 2006-2008 Priyank Gosalia  (GPL)"),
                           "comments", _("Gimmix is a graphical music player daemon (MPD) client written in C."),
                           "license", license,
                           "authors", authors,
					  "translator-credits", translators,
                           "website", APPURL,
                           "website-label", APPURL,
                           "logo", about_pixbuf,
                           "wrap-license", true,
                           NULL);
	g_object_unref (about_pixbuf);

	return;
}

int
main (int argc, char *argv[])
{
	gchar 		*path;
	char		*lang;
	int		opt;
	int		longopt_index;
	
	lang = getenv ("LC_ALL");
	if (lang==NULL || lang[0]=='\0')
		lang = getenv ("LANG");
	
	setlocale (LC_ALL, lang);
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	
	static struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"version", 0, NULL, 'v'},
		{NULL, 0, NULL, 0}
	};
	
	while ((opt = getopt_long(argc, argv, "h:v", long_options, &longopt_index)) > 0)
	{
		switch (opt)
		{
			char *vstr = NULL;
			case 'v':
				vstr = g_strdup_printf ("%s %s\n%s\n",
							g_ascii_strdown(APPNAME,strlen(APPNAME)),
							VERSION,
							"Copyright 2006, 2007, 2008 Priyank Gosalia");
				fprintf (stdout, vstr);
				g_free (vstr);
				goto cleanup;
				break;
			case 'h':
			default:
				fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
				fprintf(stderr, "  -h, --help			display this help\n");
				fprintf(stderr, "  -v, --version			version information\n");
				return 1;
		}
	}
	
	g_thread_init (NULL);
	gdk_threads_init ();

	gtk_init (&argc, &argv);
	
	path = g_strdup_printf ("%s%s", PREFIX, GLADE_FILE);
	xml = glade_xml_new (path, NULL, NULL);
	g_free (path);
	
	if (xml == NULL)
	{	
		g_error (_("Failed to initialize interface."));
		exit_cleanup ();
	}
	
	glade_xml_signal_autoconnect (xml);
	connection_box = glade_xml_get_widget (xml, "gimmix_connectionbox");
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"gimmix_connect_button")),
			"clicked",
			G_CALLBACK(cb_gimmix_connect_button_clicked),
			NULL);
	if (gimmix_config_exists())
	{
		gimmix_config_init (); /* initialize configuration */
		gimmix_interface_widgets_init ();
		gimmix_interface_disable_controls ();
		if (gimmix_connect())
		{
			gimmix_init ();
			gimmix_interface_enable_controls ();
			gtk_widget_hide (connection_box);
		}
		else
		{
			gtk_widget_show (connection_box);
		}
	}
	else
	{
		/* display the first run dialog */
		gimmix_show_firstrun_dialog (); 
	}
	
	gdk_threads_enter ();
	gtk_main ();
	gdk_threads_leave ();
	
	cleanup:
	exit_cleanup ();
	
	return 0;
}

void
exit_cleanup ()
{
	gimmix_interface_cleanup ();
	if (gmo != NULL)
		gimmix_disconnect (gmo);
	gimmix_config_free ();
	
	return;
}

