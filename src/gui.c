#include <unistd.h>
#include <gtk/gtk.h>
#include "client.h"

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

void cb_btn_func_right_clicked(GtkBuilder *builder)
{
  GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
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

void cb_btn_login_clicked(GtkButton *self, GtkDialog *dlglogin)
{
  const char *label = gtk_button_get_label(self);

  if (strcmp(label, "Login") == 0) {
    // XXX: Since the credential is carried with requests, here we cannot verify
    // the availability of account, so we just pretend to login...

    switch (gtk_dialog_run(dlglogin)) {
      case GTK_RESPONSE_OK:
        gtk_button_set_label(self, "Logout");
        break;
      default:
        break;
    }

    // We need to manually hide the login dialog
    gtk_widget_hide(GTK_WIDGET(dlglogin));
  } else if (strcmp(label, "Logout") == 0) {
    // ... and here we just pretend to logout
    gtk_button_set_label(self, "Login");
  } else {
    // Something happened
    g_warning("%s: unexpected login button label %s", __func__, label);
  }
}

/*
 * Private structure: used to pass UI elements and a message queue between
 * update_visual_elements() and do_search().
 */
struct _Elements {
  GAsyncQueue *msg_queue;
  GtkBuilder  *builder;
};

gboolean update_visual_elements(gpointer data)
{
  struct _Elements *elements = (struct _Elements *)data;

  // Timeout of bouncing
  static const unsigned int timeout = 50;

  // Internal timer
  static unsigned int timer = 0;

  // The queried person information
  Person *person = NULL;

  // Eye candy start
  if (timer == 0) {
    gtk_entry_set_progress_fraction(GTK_ENTRY(gtk_builder_get_object(elements->builder, "searchentry_main")), 0.0);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(elements->builder, "searchentry_main")), FALSE);

    // Show the text also on header bar
    gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_builder_get_object(elements->builder, "headerbar_main")), "Searching...");
  }

  if (timer < timeout) {
    gtk_entry_progress_pulse(GTK_ENTRY(gtk_builder_get_object(elements->builder, "searchentry_main")));
    timer += 1;

    // Check if the search succeeded.
    // - If yes, prepare and switch to the result page
    // - If no, display error message
    person = (Person *)g_async_queue_try_pop(elements->msg_queue);
    if (person) {
      if (person->name) {
        // Query success
        // TODO
        g_message("%s: stub, not implemented!", __func__);

        gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_builder_get_object(elements->builder, "headerbar_main")), "Search");
        goto end;
      } else {
        // Query error

        gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_builder_get_object(elements->builder, "headerbar_main")), "Search error! Please retry...");
        goto end;
      }
    } else {} // Keep going

    // Let g_timeout keep running this function
    return TRUE;
  }

  // Timed out, restore the elements to the default state
  gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_builder_get_object(elements->builder, "headerbar_main")), "Timed out, please retry.");

end:
  gtk_entry_set_progress_fraction(GTK_ENTRY(gtk_builder_get_object(elements->builder, "searchentry_main")), 0.0);
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(elements->builder, "searchentry_main")), TRUE);

  // We are also responsible to free the unused memory...
  free(person);
  free(elements);

  // Reset timer too
  timer = 0;

  // Stop g_timeout from running this function
  return FALSE;
}

void *do_search(gpointer data)
{
  struct _Elements *elements = (struct _Elements *)data;

  // Extract query text from the search entry
  const char *query_str = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(elements->builder, "searchentry_main")));

  // Just do it (this is synchronous)
  Person *person = client_query(query_str);

  if (!person) {
    // Query failure, but we still need to enqueue something to notify the UI...
    g_async_queue_push(elements->msg_queue, g_malloc0_n(1, sizeof(Person)));
  } else {
    g_async_queue_push(elements->msg_queue, person);
  }

  // Thread dies
  return NULL;
}

void cb_searchentry_main_activate(GtkBuilder *builder)
{
  // Pulse the progress bar in the search entry to show the query is ongoing
  struct _Elements *elements = malloc(sizeof(struct _Elements));
  if (!elements) {
    g_warning("%s: memory allocation failed!", __func__);
    return;
  }

  // This queue is used to pass messages between the UI update timer and the
  // searching thread...
  GAsyncQueue *queue = g_async_queue_new();

  // Zip them together for the function to update
  elements->msg_queue = queue;
  elements->builder   = builder;

  // Spawn a new thread to retrieve data from the server
  g_thread_unref(g_thread_new("search", do_search, elements));

  // NOTE: This is crucial. If we spawn a new thread to update the widget then
  // the program simply crashes because GTK+3 IS NOT THREAD SAFE. Use this to
  // keep the UI updated in the UI thread (which is the main thread of this
  // program).
  g_timeout_add(100, update_visual_elements, elements);
}

void gui_start(int *argc, char ***argv)
{
  gtk_init(argc, argv);

  GtkBuilder *builder = gtk_builder_new_from_file("res/gui.glade");
  GtkApplicationWindow *winmain =
    GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "winmain"));

  gtk_builder_connect_signals(builder, builder);

  gtk_widget_show_all(GTK_WIDGET(winmain));
  gtk_main();
}
