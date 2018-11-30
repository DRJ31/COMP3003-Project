#include "glib.h"
#include "client.h"

Person *client_query(const char *q)
{
  if (!q)
    return NULL;

  g_message("%s: stub, not implemented!", __func__);

  return NULL;
}
