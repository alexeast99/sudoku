#include <gtk/gtk.h>
#include <iostream>

int main (int argc, char **argv) {
  GtkWidget  *window;
  GtkBuilder *builder;

  gtk_init (&argc, &argv);

  builder = gtk_builder_new_from_file ("res/GUI.glade");
  window  = GTK_WIDGET (gtk_builder_get_object(builder, "application_window"));
  gtk_builder_connect_signals (builder, NULL);

  g_object_unref (G_OBJECT (builder));

  gtk_widget_show (window);
  g_signal_connect(window, "destroy", gtk_main_quit, NULL);
  gtk_main ();

  return 0;
}
