#include <glib.h>
#include "gui.h"
#include "server.h"

int main(int argc, char **argv)
{
  if (argc > 1 && strcmp("server", argv[1]) == 0) {
    server_start(&argc, &argv);
  } else {
    gui_start(&argc, &argv);
  }

  return 0;
}
