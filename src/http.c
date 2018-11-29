#include <stdlib.h>
#include "http.h"

char *http_request_wrap_message(enum HttpMethod method, const char *path, struct HttpHeaders headers, const char *message)
{
  if (!path || !message)
    return NULL;

  g_message("%s: stub, not implemented!", __func__);

  return NULL;
}

char *http_response_wrap_message(enum HttpStatusCode status, const char *message)
{
  if (!message)
    return NULL;

  g_message("%s: stub, not implemented!", __func__);

  return NULL;
}
