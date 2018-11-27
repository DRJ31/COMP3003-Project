#include <gtk/gtk.h>

// Callback functions for GTK widget connected signals
// See the GTK builder UI definition for information

void cb_btn_func_left_clicked(GtkButton *self, gpointer data)
{
  const char *curr_stack = gtk_stack_get_visible_child_name(GTK_STACK(data));

  // If currently:
  // - On the search page
  //   - If the user is not logged in, this button is hidden
  //   - If the user is logged in, clicking this button goes to the add page
  // - On the other pages, clicking this button goes back to the search page

  if (strcmp("stackpage_label_main", curr_stack) == 0) {
    //
  } else if (strcmp("stackpage_result", curr_stack) == 0
          || strcmp("stackpage_editor", curr_stack) == 0
          || strcmp("stackpage_about", curr_stack) == 0)
  {
    if (FALSE) { // TODO: STUB: User status query

    } else {

    }
  } else {
    // Something happened
  }
}

void cb_btn_func_right_clicked(GtkButton *self, gpointer data)
{
  // TODO: STUB
}

void cb_btn_login_clicked(GtkButton *self, gpointer data)
{
  // TODO: STUB
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

void cb_searchentry_main_activate(GtkSearchEntry *self, gpointer data)
{
  // TODO: STUB
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
