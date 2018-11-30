#ifndef HTTP_H
#define HTTP_H

/**
 * HTTP status code in an HTTP response.
 */
enum HttpStatusCode {
  HTTP_STATUS_OK,                     ///< HTTP 200
  HTTP_STATUS_NO_CONTENT,             ///< HTTP 204

  HTTP_STATUS_BAD_REQUEST,            ///< HTTP 400
  HTTP_STATUS_FORBIDDEN,              ///< HTTP 403
  HTTP_STATUS_NOT_FOUND,              ///< HTTP 404
  HTTP_STATUS_NOT_ACCEPTABLE,         ///< HTTP 406
  HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, ///< HTTP 415

  HTTP_STATUS_INTERNAL_SERVER_ERROR,  ///< HTTP 500
};

/**
 * HTTP method in an HTTP request.
 */
enum HttpMethod {
  HTTP_GET,    ///< HTTP GET request
  HTTP_PUT,    ///< HTTP PUT request
  HTTP_POST,   ///< HTTP POST request
  HTTP_DELETE, ///< HTTP DELETE request
};

/**
 *  HTTP headers, used in http_*_wrap_message().
 */
struct HttpHeaders {
  char *host;          ///< Content of the "Host:" header
  char *content_type;  ///< Content of the "Content-Type:" header
  char *accept;        ///< Content of the "Accept:" header
  char *authorization; ///< Content of the "Authorization:" header
};

/**
 * Construct a HTTP request.
 *
 * @param method  [in] The HTTP method (verb) to use.
 * @param path    [in] The path of resource to request.
 * @param headers [in] The HTTP headers to set.
 * @param message [in] The HTTP payload. On GET request, this field is omitted.
 * @return A string of full HTTP request. The caller is responsible to free the
 *         memory.
 */
char *http_request_wrap_message(enum HttpMethod method, const char *path, struct HttpHeaders headers, const char *message);

/**
 * Construct a HTTP response.
 *
 * @param status  [in] The HTTP status code to return.
 * @param headers [in] The HTTP headers to set.
 * @param message [in] The HTTP payload. Only on 200 OK response this field is
 *                     used.
 * @return A string of full HTTP response. The caller is responsible to free the
 *         memory.
 */
char *http_response_wrap_message(enum HttpStatusCode status, struct HttpHeaders headers, const char *message);

#endif
