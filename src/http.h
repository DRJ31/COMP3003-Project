#ifndef HTTP_H
#define HTTP_H

enum HttpStatusCode {
  HTTP_STATUS_OK,                     // 200
  HTTP_STATUS_NO_CONTENT,             // 204

  HTTP_STATUS_BAD_REQUEST,            // 400
  HTTP_STATUS_FORBIDDEN,              // 403
  HTTP_STATUS_NOT_FOUND,              // 404
  HTTP_STATUS_NOT_ACCEPTABLE,         // 406
  HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, // 415

  HTTP_STATUS_INTERNAL_SERVER_ERROR,  // 500
};

enum HttpMethod {
  HTTP_GET,
  HTTP_PUT,
  HTTP_POST,
  HTTP_DELETE,
};

struct HttpHeaders {
  char *host;
  char *content_type;
  char *accept;
  char *authorization;
};

char *http_request_wrap_message(enum HttpMethod method, const char *path, struct HttpHeaders headers, const char *message);
char *http_response_wrap_message(enum HttpStatusCode status, const char *message);

#endif
