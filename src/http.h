#ifndef HTTP_H
#define HTTP_H

/**
 * HTTP status code in an HTTP response.
 */
enum HttpStatusCode {
  HTTP_STATUS_UNKNOWN,                ///< Unknown HTTP status code
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
  HTTP_METHOD_UNKNOWN, ///< Unknown verb, used to indicate a non-HTTP message
  HTTP_GET,    ///< HTTP GET request
  HTTP_PUT,    ///< HTTP PUT request
  HTTP_POST,   ///< HTTP POST request
  HTTP_DELETE, ///< HTTP DELETE request
};

/**
 * HTTP header indicator, mainly for use in http_extract_header().
 */
enum HttpHeader {
  HTTP_HOST,          ///< "Host:"
  HTTP_CONTENT_TYPE,  ///< "Content-Type:"
  HTTP_ACCEPT,        ///< "Accept:"
  HTTP_AUTHORIZATION, ///< "Authorization:"
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

/**
 * Extract HTTP method from the request string.
 *
 * @param req [in] The HTTP request string.
 * @return HTTP method represented in HttpMethod.
 */
enum HttpMethod http_extract_request_method(const char *req);

/**
 * Extract HTTP resource path from the request string.
 *
 * @param req [in] The HTTP request string.
 * @return A string containing the HTTP path requested in the request.
 */
char *http_extract_request_path(const char *req);

/**
 * Extract HTTP statua code from the response string.
 *
 * @param req [in] The HTTP response string.
 * @return HTTP status code represented in HttpStatusCode.
 */
enum HttpStatusCode http_extract_response_status(const char *resp);

/**
 * Extract a HTTP header from the HTTP string.
 *
 * @param http [in] The HTTP message string.
 * @param header [in] The HTTP header to extract.
 * @return A string containing the header content (the part after ":"). The
 *         caller is responsible for freeing the memory.
 */
char *http_extract_header(const char *http, enum HttpHeader header);

/**
 * Extract HTTP payload from the HTTP string.
 *
 * @param http [in] The HTTP message string.
 * @return A string containing the payload (the part after "\r\n"). The
 *         caller is responsible for freeing the memory.
 */
char *http_extract_payload(const char *http);

#endif
