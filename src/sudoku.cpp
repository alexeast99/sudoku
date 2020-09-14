/*
* Date: 09/14/20
* Author: Alex Eastman
* Summary: Main program file for Sudoku
*/

#include <gtk/gtk.h>
#include <iostream>


int main (int argc, char **argv) {
  // Every GTK application needs a window. Since we're using Glade, we'll get the
  // window from a builder.
  GtkWidget  *window;
  GtkBuilder *builder;

  // Initializes everything needed to operate toolkit. Call before other functions.
  gtk_init (&argc, &argv);

  // Get the builder from Glade-generate UI definition. Get window from builder.
  builder = gtk_builder_new_from_file ("res/GUI.glade");
  window  = GTK_WIDGET (gtk_builder_get_object(builder, "application_window"));

  // De-reference builder after it's been used. References to builder should go
  // before this line.
  g_object_unref (G_OBJECT (builder));

  // Show the window. Stop application when window is closed. Start main loop.
  gtk_widget_show (window);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_main ();

  return 0;
}
