#include <glib.h>
#include "gui.h"

int main(int argc, char **argv)
{
  if (argc > 1) {
    if (strcmp("server", argv[1]) == 0) {
      g_message("server not implemented!");
    }
  } else {
    gui_start(&argc, &argv);
  }

  return 0;
}
