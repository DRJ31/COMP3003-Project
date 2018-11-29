#include <stdio.h>
#include "glib.h"

void server_start(int *argc, char ***argv)
{
  if (*argc < 4) {
    printf("Usage: %s server <address> <port>\n", *argv[0]);
    return;
  }

  g_message("%s: stub, not implemented!", __func__);
}
