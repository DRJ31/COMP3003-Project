#include <gtk/gtk.h>

// Callback functions for GTK widget connected signals
// See the GTK builder UI definition for information

void cb_box_show_change_btn_func_left(GtkWidget *self, GtkButton *btn_func_left)
{
  const char *stack_name = gtk_stack_get_visible_child_name(GTK_STACK(gtk_widget_get_parent(self)));

  // Set left button state. If currently:
  // - On the search page, clicking this button goes to the add page
  // - On the other pages, clicking this button goes back to the search page

  GList *children = gtk_container_get_children(GTK_CONTAINER(btn_func_left));
  GtkImage *icon = g_list_nth_data(children, 0);
  g_list_free(children);

  if (strcmp(stack_name, "stackpage_search") == 0) {
    gtk_image_set_from_icon_name(icon, "document-new", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_show(GTK_WIDGET(btn_func_left));
  } else if (strcmp(stack_name, "stackpage_result") == 0
          || strcmp(stack_name, "stackpage_editor") == 0
          || strcmp(stack_name, "stackpage_about") == 0)
  {
    gtk_image_set_from_icon_name(icon, "go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_show(GTK_WIDGET(btn_func_left));
  } else {
    // Something happened
    g_warning("%s: unexpected stack page name %s", __func__, stack_name);
  }
}

void cb_box_show_change_btn_func_right(GtkWidget *self, GtkButton *btn_func_right)
{
  const char *stack_name = gtk_stack_get_visible_child_name(GTK_STACK(gtk_widget_get_parent(self)));

  // Set right button state. If currently:
  // - On the search page, clicking this button goes to the about page
  // - On the result page
  //   - If the user is not logged in, this button is hidden
  //   - If the user is logged in, clicking this button goes to the edit page
  // - On the add / edit page, clicking this button submits data
  // - On the about page, this button is hidden

  GList *children = gtk_container_get_children(GTK_CONTAINER(btn_func_right));
  GtkImage *icon = g_list_nth_data(children, 0);
  g_list_free(children);

  if (strcmp(stack_name, "stackpage_search") == 0) {
    gtk_image_set_from_icon_name(icon, "help-about", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_show(GTK_WIDGET(btn_func_right));
  } else if (strcmp(stack_name, "stackpage_result") == 0) {
    gtk_image_set_from_icon_name(icon, "document-edit", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_show(GTK_WIDGET(btn_func_right));
  } else if (strcmp(stack_name, "stackpage_editor") == 0) {
    gtk_image_set_from_icon_name(icon, "document-send", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_show(GTK_WIDGET(btn_func_right));
  } else if (strcmp(stack_name, "stackpage_about") == 0) {
    gtk_widget_hide(GTK_WIDGET(btn_func_right));
  } else {
    // Something happened
    g_warning("%s: unexpected stack page name %s", __func__, stack_name);
  }
}

void cb_btn_func_left_clicked(GtkStack *stack)
{
  const char *stack_name = gtk_stack_get_visible_child_name(stack);

  // Perform left button action. If currently:
  // - On the search page, clicking this button goes to the add page
  // - On the other pages, clicking this button goes back to the search page

  if (strcmp(stack_name, "stackpage_search") == 0) {
    gtk_stack_set_visible_child_name(stack, "stackpage_editor");
  } else if (strcmp(stack_name, "stackpage_result") == 0
          || strcmp(stack_name, "stackpage_editor") == 0
          || strcmp(stack_name, "stackpage_about") == 0)
  {
    gtk_stack_set_visible_child_name(stack, "stackpage_search");
  } else {
    // Something happened
    g_warning("%s: unexpected stack name %s", __func__, stack_name);
  }
}

void cb_btn_func_right_clicked(GtkStack *stack)
{
  const char *stack_name = gtk_stack_get_visible_child_name(stack);

  // Perform right button action. If currently:
  // - On the search page, clicking this button goes to the about page
  // - On the result page, clicking this button goes to the edit page
  // - On the add / edit page, clicking this button submits data
  // - On the about page, this button is hidden

  if (strcmp(stack_name, "stackpage_search") == 0) {
    gtk_stack_set_visible_child_name(stack, "stackpage_about");
  } else if (strcmp(stack_name, "stackpage_result") == 0) {
    gtk_stack_set_visible_child_name(stack, "stackpage_editor");
  } else if (strcmp(stack_name, "stackpage_editor") == 0) {
    // TODO: Submit data
    g_message("%s: data submission not implemented!", __func__);
  } else if (strcmp(stack_name, "stackpage_about") == 0) {
    // Do nothing (the button is hidden)
  } else {
    // Something happened
    g_warning("%s: unexpected stack name %s", __func__, stack_name);
  }
}

void cb_btn_login_clicked(GtkButton *self, gpointer data)
{
  // TODO: STUB
  g_message("%s: not implemented!", __func__);
}

void cb_searchentry_main_activate(GtkSearchEntry *self, gpointer data)
{
  // TODO: STUB
  g_message("%s: not implemented!", __func__);
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
