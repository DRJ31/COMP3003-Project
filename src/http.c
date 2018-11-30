#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "http.h"

// HTTP version. This implementation sticks to HTTP/1.1, so there is no way to
// change this string...
#define HTTP_VERSION "HTTP/1.1"

/* String constant of HTTP contents for internal use */

static const char *http_status_ok         = HTTP_VERSION " 200 OK";
static const char *http_status_no_content = HTTP_VERSION " 204 No Content";

static const char *http_status_bad_request            = HTTP_VERSION " 400 Bad Request";
static const char *http_status_forbidden              = HTTP_VERSION " 403 Forbidden";
static const char *http_status_not_found              = HTTP_VERSION " 404 Not Found";
static const char *http_status_not_acceptable         = HTTP_VERSION " 406 Not Acceptable";
static const char *http_status_unsupported_media_type = HTTP_VERSION " 415 Unsupported Media Type";

static const char *http_status_internal_server_error = HTTP_VERSION " 500 Server Internal Error";

static const char *http_get    = "GET";
static const char *http_put    = "PUT";
static const char *http_post   = "POST";
static const char *http_delete = "DELETE";

static const char *http_header_host          = "Host: ";
static const char *http_header_content_type  = "Content-Type: ";
static const char *http_header_accept        = "Accept: ";
static const char *http_header_authorization = "Authorization: ";


/********** Private APIs **********/


/**
 * Convert enumeration HttpMethod to string.
 *
 * @param method [in] HttpMethod to convert.
 * @return A string constant representing the HTTP method. The caller should not
 *         free the string!
 */
static const char *http_method_to_string(enum HttpMethod method)
{
  const char *ret = NULL;

  switch (method) {
    case HTTP_GET:
      ret = http_get;
      break;
    case HTTP_PUT:
      ret = http_put;
      break;
    case HTTP_POST:
      ret = http_post;
      break;
    case HTTP_DELETE:
      ret = http_delete;
      break;
    default:
      break; // Remain ret to be NULL
  }

  return ret;
}

/**
 * Convert enumeration HttpStatusCode to string.
 *
 * @param method [in] HttpStatusCode to convert.
 * @return A string constant representing the HTTP status. The caller should not
 *         free the string!
 */
static const char *http_status_to_string(enum HttpStatusCode status)
{
  const char *ret = NULL;

  switch (status) {
    case HTTP_STATUS_OK:
      ret = http_status_ok;
      break;
    case HTTP_STATUS_NO_CONTENT:
      ret = http_status_no_content;
      break;
    case HTTP_STATUS_BAD_REQUEST:
      ret = http_status_bad_request;
      break;
    case HTTP_STATUS_FORBIDDEN:
      ret = http_status_forbidden;
      break;
    case HTTP_STATUS_NOT_FOUND:
      ret = http_status_not_found;
      break;
    case HTTP_STATUS_NOT_ACCEPTABLE:
      ret = http_status_not_acceptable;
      break;
    case HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:
      ret = http_status_unsupported_media_type;
      break;
    case HTTP_STATUS_INTERNAL_SERVER_ERROR:
      ret = http_status_internal_server_error;
      break;
    default:
      break; // Remain ret to be NULL
  }

  return ret;
}

/**
 * Construct a HTTP header line for requesting.
 *
 * This gives a string like:
 *
 * ```http
 * GET /path HTTP/1.1
 * ```
 *
 * The HTTP version is defined in the HTTP_VERSION macro constant, and thus is
 * not adjustable in runtime.
 *
 * @param method [in] The HTTP method of request.
 * @param path   [in] The path to resource the request is requesting for.
 * @return A string containing the whole HTTP header line. The caller is
 *         responsible to free the memory.
 */
static char *http_request_header_line_new(const char *method, const char *path)
{
  if (!method || !path)
    return NULL;

  size_t ret_size = strlen(method) + strlen(" ")
                  + strlen(path) + strlen(" ")
                  + strlen(HTTP_VERSION)
                  + 1; // + 1 NUL

  char *ret = malloc(ret_size);
  if (!ret) {
    g_warning("%s:%d %s: memory allocation failed!", __FILE__, __LINE__, __func__);
    return NULL;
  }

  snprintf(ret, ret_size, "%s %s %s", method, path, HTTP_VERSION);

  return ret;
}

/**
 * Construct a HTTP header line for responsing.
 *
 * This gives a string like:
 *
 * ```http
 * HTTP/1.1 200 OK
 * ```
 *
 * The HTTP version is defined in the HTTP_VERSION macro constant, and thus is
 * not adjustable in runtime.
 *
 * @param status [in] The HTTP status of request.
 * @return A string containing the whole HTTP header line. The caller is
 *         responsible to free the memory.
 */
static char *http_response_header_line_new(enum HttpStatusCode status)
{
  const char *status_str = http_status_to_string(status);
  size_t ret_size = strlen(status_str) + 1; // + 1 NUL

  char *ret = malloc(ret_size);
  if (!ret) {
    g_warning("%s:%d %s: memory allocation failed!", __FILE__, __LINE__, __func__);
    return NULL;
  }

  snprintf(ret, ret_size, "%s", status_str);

  return ret;
}

/**
 * Construct a group of HTTP headers in a string.
 *
 * Depending on the input in `headers`, his gives a string like:
 *
 * ```http
 * Host: foo.bar.baz
 * Content-Type: application/json
 * ```
 *
 * Note that this function does not append the HTTP header end mark (i.e.
 * "\r\n") to the output string. You need to manually add this when you
 * construct the final output of HTTP!
 *
 * @param headers [in] The content of headers.
 * @return A string of the HTTP header list. The caller is responsible to free
 *         the memory.
 */
static char *http_headers_string_new(struct HttpHeaders headers)
{
  size_t ret_size = 0;

  // Calculate the buffer size to allocate
  if (headers.host) {
    ret_size += strlen(http_header_host) + strlen(headers.host) + strlen("\n");
  }
  if (headers.content_type) {
    ret_size += strlen(http_header_content_type) + strlen(headers.content_type) + strlen("\n");
  }
  if (headers.accept) {
    ret_size += strlen(http_header_accept) + strlen(headers.accept) + strlen("\n");
  }
  if (headers.authorization) {
    ret_size += strlen(http_header_authorization) + strlen(headers.authorization) + strlen("\n");
  }

  char *ret = malloc(ret_size + 1); // + 1 NUL
  if (!ret) {
    g_warning("%s:%d %s: memory allocation failed!", __FILE__, __LINE__, __func__);
    return NULL;
  }

  // NOTE: This is required for strcat() to work properly!
  ret[0] = '\0';

  if (headers.host) {
    strcat(ret, http_header_host);
    strcat(ret, headers.host);
    strcat(ret, "\n");
  }
  if (headers.content_type) {
    strcat(ret, http_header_content_type);
    strcat(ret, headers.content_type);
    strcat(ret, "\n");
  }
  if (headers.accept) {
    strcat(ret, http_header_accept);
    strcat(ret, headers.accept);
    strcat(ret, "\n");
  }
  if (headers.authorization) {
    strcat(ret, http_header_authorization);
    strcat(ret, headers.authorization);
    strcat(ret, "\n");
  }

  return ret;
}


/********** Public APIs **********/


char *http_request_wrap_message(enum HttpMethod method, const char *path, struct HttpHeaders headers, const char *message)
{
  if (!path)
    return NULL;

  // On HTTP GET, payload is omitted
  if (method != HTTP_GET)
    if (!message)
      return NULL;

  char *header_line = http_request_header_line_new(http_method_to_string(method), path);
  if (!header_line) {
    g_warning("%s:%d %s: http_request_header_line_new failed!", __FILE__, __LINE__, __func__);
    return NULL;
  }

  char *http_headers = http_headers_string_new(headers);
  if (!http_headers) {
    g_warning("%s:%d %s: http_headers_string_new failed!", __FILE__, __LINE__, __func__);
    free(header_line);
    return NULL;
  }

  size_t message_len = strlen(header_line) + strlen("\n")
                     + strlen(http_headers) + strlen("\r\n")
                     + 1; // + 1 NUL

  if (method != HTTP_GET)
    message_len += strlen(message);

  char *ret = malloc(message_len);
  if (!ret) {
    g_warning("%s:%d %s: memory allocation failed!", __FILE__, __LINE__, __func__);
    free(http_headers);
    free(header_line);
    return NULL;
  }

  snprintf(ret, message_len, "%s\n%s\r\n%s", header_line, http_headers, method == HTTP_GET ? "" : message);

  // Free unused memory
  free(http_headers);
  free(header_line);

  return ret;
}

char *http_response_wrap_message(enum HttpStatusCode status, struct HttpHeaders headers, const char *message)
{
  if (!message)
    return NULL;

  char *header_line = http_response_header_line_new(status);
  if (!header_line) {
    g_warning("%s:%d %s: http_response_header_line_new failed!", __FILE__, __LINE__, __func__);
    return NULL;
  }

  char *http_headers = http_headers_string_new(headers);
  if (!http_headers) {
    g_warning("%s:%d %s: http_headers_string_new failed!", __FILE__, __LINE__, __func__);
    free(header_line);
    return NULL;
  }

  size_t message_len = strlen(header_line) + strlen("\n")
                     + strlen(http_headers) + strlen("\r\n")
                     + 1; // + 1 NUL

  // Only on HTTP 200 OK response, payload is carried
  if (status == HTTP_STATUS_OK)
    message_len += strlen(message);

  char *ret = malloc(message_len);
  if (!ret) {
    g_warning("%s:%d %s: memory allocation failed!", __FILE__, __LINE__, __func__);
    free(http_headers);
    free(header_line);
    return NULL;
  }

  snprintf(ret, message_len, "%s\n%s\r\n%s", header_line, http_headers, status == HTTP_STATUS_OK ? message : "");

  // Free unused memory
  free(http_headers);
  free(header_line);

  return ret;
}

#ifdef TEST
int main(void)
{
  {
    char *request = http_request_wrap_message(
      HTTP_GET,
      "/test",
      (struct HttpHeaders) {
        .host = "foo.bar",
        .accept = "application/json",
        .content_type = NULL,
        .authorization = NULL,
      },
      NULL
    );

    char *response = http_response_wrap_message(
      HTTP_STATUS_OK,
      (struct HttpHeaders) {
        .host = "foo.bar",
        .accept = NULL,
        .content_type = "application/json",
        .authorization = NULL,
      },
      "{\"name\":\"test\",\"mobile\":[\"+86 131 2345 6789\"]}"
    );

    puts("===== Request =====");
    puts(request);
    puts("===================");

    puts("===== Response =====");
    puts(response);
    puts("====================");

    free(request);
    free(response);
  }
}
#endif
