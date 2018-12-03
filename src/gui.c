#include <gtk/gtk.h>
#include "login.h"
#include "client.h"

/*
 * Private structure: used to pass UI elements and a message queue between
 * an action function and a UI update function.
 */
struct _Elements {
  GAsyncQueue *msg_queue;
  GtkBuilder  *builder;
};

// Callback functions for GTK widget connected signals
// See the GTK builder UI definition for information

void cb_box_show_change_visual(GtkBuilder *builder)
{
  GtkWidget *headerbar_main  = GTK_WIDGET(gtk_builder_get_object(builder, "headerbar_main"));
  GtkWidget *btn_func_left   = GTK_WIDGET(gtk_builder_get_object(builder, "btn_func_left"));
  GtkWidget *btn_func_right  = GTK_WIDGET(gtk_builder_get_object(builder, "btn_func_right"));
  GtkWidget *icon_func_left  = GTK_WIDGET(gtk_builder_get_object(builder, "icon_func_left"));
  GtkWidget *icon_func_right = GTK_WIDGET(gtk_builder_get_object(builder, "icon_func_right"));

  const char *stack_name = gtk_stack_get_visible_child_name(
    GTK_STACK(gtk_builder_get_object(builder, "stack"))
  );

  // Set left button state, if currently:
  // - On the search page, clicking this button goes to the add page
  // - On the other pages, clicking this button goes back to the search page
  //
  // Set right button state, if currently:
  // - On the search page, clicking this button goes to the about page
  // - On the result page
  //   - If the user is not logged in, this button is hidden
  //   - If the user is logged in, clicking this button goes to the edit page
  // - On the add / edit page, clicking this button submits data
  // - On the about page, this button is hidden

  if (strcmp(stack_name, "stackpage_search") == 0) {
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Search");

    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_left), "document-new", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_right), "help-about", GTK_ICON_SIZE_LARGE_TOOLBAR);

    if (is_logged_in())
      gtk_widget_show(btn_func_left);
    gtk_widget_show(btn_func_right);
  } else if (strcmp(stack_name, "stackpage_result") == 0) {
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Result");

    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_left), "go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_right), "document-edit", GTK_ICON_SIZE_LARGE_TOOLBAR);

    gtk_widget_show(btn_func_left);
    // gtk_widget_show(btn_func_right); // XXX: Not implemented
    gtk_widget_hide(btn_func_right);
  } else if (strcmp(stack_name, "stackpage_editor") == 0) {
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Editor");

    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_left), "go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_right), "document-send", GTK_ICON_SIZE_LARGE_TOOLBAR);

    gtk_widget_show(btn_func_left);
    gtk_widget_show(btn_func_right);
  } else if (strcmp(stack_name, "stackpage_about") == 0) {
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "About");

    gtk_image_set_from_icon_name(GTK_IMAGE(icon_func_left), "go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR);

    gtk_widget_show(btn_func_left);
    gtk_widget_hide(btn_func_right);
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

Person *editor_collect(GtkBuilder *builder)
{
  GtkEntry *entry_name_main = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name_main"));
  GtkEntry *entry_name_aka  = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name_aka"));
  GtkEntry *entry_phone_1   = GTK_ENTRY(gtk_builder_get_object(builder, "entry_phone_1"));
  GtkEntry *entry_phone_2   = GTK_ENTRY(gtk_builder_get_object(builder, "entry_phone_2"));
  GtkEntry *entry_fax_1     = GTK_ENTRY(gtk_builder_get_object(builder, "entry_fax_1"));
  GtkEntry *entry_fax_2     = GTK_ENTRY(gtk_builder_get_object(builder, "entry_fax_2"));
  GtkEntry *entry_note_1    = GTK_ENTRY(gtk_builder_get_object(builder, "entry_note_1"));
  GtkEntry *entry_note_2    = GTK_ENTRY(gtk_builder_get_object(builder, "entry_note_2"));

  const char *name_main = gtk_entry_get_text(entry_name_main);
  const char *name_aka  = gtk_entry_get_text(entry_name_aka);
  const char *phone_1   = gtk_entry_get_text(entry_phone_1);
  const char *phone_2   = gtk_entry_get_text(entry_phone_2);
  const char *fax_1     = gtk_entry_get_text(entry_fax_1);
  const char *fax_2     = gtk_entry_get_text(entry_fax_2);
  const char *note_1    = gtk_entry_get_text(entry_note_1);
  const char *note_2    = gtk_entry_get_text(entry_note_2);

  Person *person = g_new0(Person, 1);

  if (strlen(name_main) > 0) {
    if (strlen(name_aka) > 0) {
      person->name = g_malloc0_n(2 + 1, sizeof(char *)); // + 1 NULL
      person->name[0] = g_strdup(name_main);
      person->name[1] = g_strdup(name_aka);
    } else {
      person->name = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->name[0] = g_strdup(name_main);
    }
  } else {
    if (strlen(name_aka) > 0) {
      person->name = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->name[0] = g_strdup(name_aka);
    } else {}
  }

  if (strlen(phone_1) > 0) {
    if (strlen(phone_2) > 0) {
      person->mobile = g_malloc0_n(2 + 1, sizeof(char *)); // + 1 NULL
      person->mobile[0] = g_strdup(phone_1);
      person->mobile[1] = g_strdup(phone_2);
    } else {
      person->mobile = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->mobile[0] = g_strdup(phone_1);
    }
  } else {
    if (strlen(phone_2) > 0) {
      person->mobile = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->mobile[0] = g_strdup(phone_2);
    } else {}
  }

  if (strlen(fax_1) > 0) {
    if (strlen(fax_2) > 0) {
      person->fax = g_malloc0_n(2 + 1, sizeof(char *)); // + 1 NULL
      person->fax[0] = g_strdup(fax_1);
      person->fax[1] = g_strdup(fax_2);
    } else {
      person->fax = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->fax[0] = g_strdup(fax_1);
    }
  } else {
    if (strlen(fax_2) > 0) {
      person->fax = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->fax[0] = g_strdup(fax_2);
    } else {}
  }

  if (strlen(note_1) > 0) {
    if (strlen(note_2) > 0) {
      person->note = g_malloc0_n(2 + 1, sizeof(char *)); // + 1 NULL
      person->note[0] = g_strdup(note_1);
      person->note[1] = g_strdup(note_2);
    } else {
      person->note = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->note[0] = g_strdup(note_1);
    }
  } else {
    if (strlen(note_2) > 0) {
      person->note = g_malloc0_n(1 + 1, sizeof(char *)); // + 1 NULL
      person->note[0] = g_strdup(note_2);
    } else {}
  }

  return person;
}

gboolean update_visual_elements_on_submission(gpointer data)
{
  struct _Elements *elements = (struct _Elements *)data;

  gboolean *ret = g_async_queue_try_pop(elements->msg_queue);

  if (ret) {
    // Widget to operate on
    GtkWidget *headerbar_main = GTK_WIDGET(gtk_builder_get_object(elements->builder, "headerbar_main"));
    GtkWidget *btn_func_right = GTK_WIDGET(gtk_builder_get_object(elements->builder, "btn_func_right"));

    if (*ret == TRUE) {
      // Update succeeded
      gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Success on update.");
      gtk_widget_set_sensitive(btn_func_right, TRUE);

      g_free(elements);
      return FALSE;
    } else {
      // Update failed
      gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Failed to update! Please try again...");
      gtk_widget_set_sensitive(btn_func_right, TRUE);

      g_free(elements);
      return FALSE;
    }
  }

  // Keep it running
  return TRUE;
}

enum _SubmitAction {
  _ADD,
  _EDIT,
};

/*
 * Private struct used in do_submit().
 */
struct _Submit {
  GAsyncQueue *msg_queue;
  enum _SubmitAction action;
  Person *person;
};

void *do_submit(gpointer data)
{
  struct _Submit *submit = (struct _Submit *)data;
  gboolean *ret = g_malloc0(sizeof(gboolean));

  if (submit->action == _ADD) {
    *ret = client_add(submit->person);
  } else if (submit->action == _EDIT) {
    *ret = client_edit(submit->person->name[0], submit->person); // XXX
  }

  // Push the result to the async queue to notify UI thread
  g_async_queue_push(submit->msg_queue, ret);

  // Free unused data
  // person_free(submit->person); // TODO: FIXME
  g_free(submit);

  // Thread dies
  return NULL;
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
    // Change header bar title to indicate process
    GtkWidget *headerbar_main = GTK_WIDGET(gtk_builder_get_object(builder, "headerbar_main"));
    GtkWidget *btn_func_right = GTK_WIDGET(gtk_builder_get_object(builder, "btn_func_right"));

    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Submitting...");
    gtk_widget_set_sensitive(btn_func_right, FALSE);

    // Collect information in editor
    Person *person = editor_collect(builder);

    // Message queue used between the action thread and UI thread
    GAsyncQueue *queue = g_async_queue_new();

    // Spawn a new thread to do actual work
    struct _Submit *submit = g_new(struct _Submit, 1);
    submit->msg_queue = queue;
    submit->action    = _ADD;  // FIXME
    submit->person    = person;

    g_thread_unref(g_thread_new("submit", do_submit, submit));

    // Wait for a response to update UI
    struct _Elements *elements = g_new(struct _Elements, 1);
    elements->msg_queue = queue;
    elements->builder   = builder;

    g_idle_add(update_visual_elements_on_submission, elements);
  } else if (strcmp(stack_name, "stackpage_about") == 0) {
    // Do nothing (the button is hidden)
  } else {
    // Something happened
    g_warning("%s: unexpected stack name %s", __func__, stack_name);
  }
}

void cb_btn_login_clicked(GtkButton *self, GtkBuilder *builder)
{
  GtkWidget *btn_func_left  = GTK_WIDGET(gtk_builder_get_object(builder, "btn_func_left"));
  GtkEntry *entry_username = GTK_ENTRY(gtk_builder_get_object(builder, "entry_username"));
  GtkEntry *entry_password = GTK_ENTRY(gtk_builder_get_object(builder, "entry_password"));
  GtkDialog *dlglogin = GTK_DIALOG(gtk_builder_get_object(builder, "dlglogin"));

  const char *label = gtk_button_get_label(self);

  if (strcmp(label, "Login") == 0) {
    // XXX: Since the credential is carried with requests, here we cannot verify
    // the availability of account, so we just pretend to login...

    switch (gtk_dialog_run(dlglogin)) {
      case GTK_RESPONSE_OK:
        pb_login(gtk_entry_get_text(entry_username), gtk_entry_get_text(entry_password));
        gtk_widget_show(btn_func_left);

        gtk_button_set_label(self, "Logout");
        break;
      default:
        break;
    }

    // We need to manually hide the login dialog
    gtk_widget_hide(GTK_WIDGET(dlglogin));
  } else if (strcmp(label, "Logout") == 0) {
    // ... and here we just pretend to logout
    pb_logout();
    gtk_widget_hide(btn_func_left);

    gtk_button_set_label(self, "Login");
  } else {
    // Something happened
    g_warning("%s: unexpected login button label %s", __func__, label);
  }

  // Clear the entries
  gtk_entry_set_text(entry_username, "");
  gtk_entry_set_text(entry_password, "");
}

void update_result_page(GtkBuilder *builder, Person *person)
{
  if (!person)
    return;

  GtkLabel *label_name_main = GTK_LABEL(gtk_builder_get_object(builder, "label_name_main"));
  GtkLabel *label_name_aka  = GTK_LABEL(gtk_builder_get_object(builder, "label_name_aka"));
  GtkLabel *label_phone_1   = GTK_LABEL(gtk_builder_get_object(builder, "label_phone_1"));
  GtkLabel *label_phone_2   = GTK_LABEL(gtk_builder_get_object(builder, "label_phone_2"));
  GtkLabel *label_fax_1     = GTK_LABEL(gtk_builder_get_object(builder, "label_fax_1"));
  GtkLabel *label_fax_2     = GTK_LABEL(gtk_builder_get_object(builder, "label_fax_2"));
  GtkLabel *label_note_1    = GTK_LABEL(gtk_builder_get_object(builder, "label_note_1"));
  GtkLabel *label_note_2    = GTK_LABEL(gtk_builder_get_object(builder, "label_note_2"));

  // Clear all labels prior to update
  gtk_label_set_text(label_name_main, "");
  gtk_label_set_text(label_name_aka, "");
  gtk_label_set_text(label_phone_1, "");
  gtk_label_set_text(label_phone_2, "");
  gtk_label_set_text(label_fax_1, "");
  gtk_label_set_text(label_fax_2, "");
  gtk_label_set_text(label_note_1, "");
  gtk_label_set_text(label_note_2, "");

  // Name (only the first one or two is shown, same as the next categories)
  if (person->name) {
    if (person->name[0]) {
      gtk_label_set_text(label_name_main, person->name[0]);
      if (person->name[1]) {
        gtk_label_set_text(label_name_aka, person->name[1]);
      }
    } else {
      gtk_label_set_text(label_name_main, "N/A");
    }
  }

  // Phone
  if (person->mobile) {
    if (person->mobile[0]) {
      gtk_label_set_text(label_phone_1, person->mobile[0]);
      if (person->mobile[1]) {
        gtk_label_set_text(label_phone_2, person->mobile[1]);
      }
    }
  }

  // Fax
  if (person->fax) {
    if (person->fax[0]) {
      gtk_label_set_text(label_fax_1, person->fax[0]);
      if (person->fax[1]) {
        gtk_label_set_text(label_fax_2, person->fax[1]);
      }
    }
  }

  // Note
  if (person->note) {
    if (person->note[0]) {
      gtk_label_set_text(label_note_1, person->note[0]);
      if (person->note[1]) {
        gtk_label_set_text(label_note_2, person->note[1]);
      }
    }
  }
}

gboolean update_visual_elements_on_search(gpointer data)
{
  struct _Elements *elements = (struct _Elements *)data;

  // Timeout of bouncing
  static const unsigned int timeout = 50;

  // Internal timer
  static unsigned int timer = 0;

  // Widgets to manipulate on
  GtkWidget *searchentry_main = GTK_WIDGET(gtk_builder_get_object(elements->builder, "searchentry_main"));
  GtkWidget *headerbar_main   = GTK_WIDGET(gtk_builder_get_object(elements->builder, "headerbar_main"));

  // The queried person information
  Person *person = NULL;

  // Eye candy start
  if (timer == 0) {
    gtk_entry_set_progress_fraction(GTK_ENTRY(searchentry_main), 0.0);
    gtk_widget_set_sensitive(searchentry_main, FALSE);

    // Show the text also on header bar
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Searching...");
  }

  if (timer < timeout) {
    gtk_entry_progress_pulse(GTK_ENTRY(searchentry_main));
    timer += 1;

    // Check if the search succeeded.
    // - If yes, prepare and switch to the result page
    // - If no, display error message
    person = (Person *)g_async_queue_try_pop(elements->msg_queue);
    if (person) {
      if (person->name) {
        // Query success

        update_result_page(elements->builder, person);
        gtk_stack_set_visible_child_name(GTK_STACK(gtk_builder_get_object(elements->builder, "stack")), "stackpage_result");
        gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Result");

        goto end;
      } else {
        // Query error
        gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Search error! Please retry...");

        goto end;
      }
    } else {} // Keep going

    // Let g_timeout keep running this function
    return TRUE;
  }

  // Timed out, restore the elements to the default state
  gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar_main), "Timed out, please retry.");

end:
  gtk_entry_set_progress_fraction(GTK_ENTRY(searchentry_main), 0.0);
  gtk_widget_set_sensitive(searchentry_main, TRUE);

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

  GtkWidget *searchentry_main = GTK_WIDGET(gtk_builder_get_object(elements->builder, "searchentry_main"));

  // Extract query text from the search entry
  const char *query_str = gtk_entry_get_text(GTK_ENTRY(searchentry_main));

  // Just do it (this is synchronous)
  Person *person = client_query(query_str);

  if (!person) {
    // Query failure, but we still need to enqueue something to notify the UI...
    g_async_queue_push(elements->msg_queue, g_new0(Person, 1));
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
  g_timeout_add(100, update_visual_elements_on_search, elements);
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
