#include <gtk/gtk.h>

// Callback functions for GTK widget connected signals
// See the GTK builder UI definition for information

void cb_togglebtn_popover_main_toggled(GtkToggleButton *self, gpointer data)
{
  gtk_popover_popup(GTK_POPOVER(data));
}

void cb_popover_main_closed(GtkPopover *self, gpointer data)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), FALSE);
}

void cb_togglebtn_search_toggled(GtkToggleButton *self, gpointer data)
{
  gboolean is_on = gtk_toggle_button_get_active(self);
  gboolean is_searching = gtk_search_bar_get_search_mode(GTK_SEARCH_BAR(data));

  if (is_on) {
    if (!is_searching)
      gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(data), TRUE);
  } else {
    if (is_searching)
      gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(data), FALSE);
  }
}

void cb_searchentry_stop_search(GtkSearchEntry *self, gpointer data)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), FALSE);
}

void cb_btn_about_clicked(GtkButton *self, gpointer data)
{
  gtk_popover_popdown(GTK_POPOVER(data));

  // NOTE: Here we manually create a GtkAboutDialog instead of in GTK builder
  // because when the dialog is destroyed it is not very graceful to rebuild it
  // again from the GTK builder, so we just create one.

  static const gchar *authors[] = {
    "Renjie Deng <l630003010@mail.uic.edu.hk>",
    "Minyong Li <l630003027@mail.uic.edu.hk>",
    "Zhenghao Wu <l630003054@mail.uic.edu.hk>",
    NULL,
  };

  GtkAboutDialog *about = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
  gtk_about_dialog_set_logo_icon_name(about, "x-office-address-book");
  gtk_about_dialog_set_program_name(about, "Phone Book");
  gtk_about_dialog_set_version(about, "0.1");
  gtk_about_dialog_set_comments(about, "A COMP3003 Data Communication and Networking course project");
  gtk_about_dialog_set_license_type(about, GTK_LICENSE_MIT_X11);
  gtk_about_dialog_set_authors(about, authors);

  gtk_widget_show(GTK_WIDGET(about));
}

void cb_winmain_destroy(GtkApplicationWindow *self, gpointer data)
{
  gtk_main_quit();
}

void gui_start(int *argc, char ***argv)
{
  gtk_init(argc, argv);

  GtkBuilder *builder = gtk_builder_new_from_file("res/gui.glade");
  GtkApplicationWindow *winmain =
    GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "winmain"));

  gtk_builder_connect_signals(builder, NULL);

  gtk_widget_show_all(GTK_WIDGET(winmain));
  gtk_main();
}
