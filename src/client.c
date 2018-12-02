#include <glib.h>
#include "http.h"
#include "json.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUFSIZE 1024

// Default server address and port. To adjust these settings, use environment
// variable PB_SERVER and PB_PORT instead.
static const char *server = "127.0.0.1";
static const char *port = "8000";

char *msg_transfer(const char *host, const char *port, const char *msg)
{
  char buf[BUFSIZE] = {0}; // Receive respond from server
  char *result;            // Store result

  // Create file descriptor
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
  {
    perror("Socket error");
    return NULL;
  }

  // Connect to server
  struct sockaddr_in c_addr;
  c_addr.sin_family = AF_INET;
  c_addr.sin_port = htons(atoi(port));               // Modify server port here
  inet_pton(AF_INET, host, &c_addr.sin_addr.s_addr); // Modify server address here

  int ret = connect(fd, (struct sockaddr *)&c_addr, sizeof(c_addr));
  if (ret == -1)
  {
    perror("Connect error");
    return NULL;
  }

  // Send message
  write(fd, msg, strlen(msg)); // Send data to server
  // Receive message
  ssize_t len = read(fd, buf, sizeof(buf));
  if (len == -1)
  {
    perror("Read error");
    return NULL;
  }
  result = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
  strcpy(result, buf);
  close(fd);
  return result;
}

Person *client_query(const char *q)
{
  if (!q)
    return NULL;

  // URL encode the query string first
  char *escaped_q = g_uri_escape_string(q, NULL, TRUE);

  // Path to query
  size_t path_size = strlen("/") + strlen(escaped_q) + 1; // + 1 NUL
  char *path = g_malloc(path_size);
  g_snprintf(path, path_size, "/%s", escaped_q);

  // Check environment variable for updated server and port
  const char *env_server = g_getenv("PB_SERVER");
  const char *env_port = g_getenv("PB_PORT");

  if (env_server)
    server = env_server;
  if (env_port)
    port = env_port;

  // Construct the "Host:" HTTP header content (combine server and port)
  // I should have decided to implememnt HTTP/1.0...
  size_t host_size = strlen(server) + strlen(":") + strlen(port) + 1; // + 1 NUL
  char *server_host = g_malloc(host_size);
  g_snprintf(server_host, host_size, "%s:%s", server, port);

  char *http = http_request_wrap_message(
      HTTP_GET,
      path,
      (struct HttpHeaders){
          .host = server_host,
          .accept = "application/json",
      },
      NULL);

  g_message("HTTP request: ===\n%s\n===", http);

  // Send it over socket
  Person *person = NULL;
  char *msg = msg_transfer(server, port, http);
  if (msg != NULL) {
    person = json_string_to_person(http_extract_payload(msg));
  }

  // g_message("%s: stub, not implemented!", __func__);

  // Free unused memory
  free(server_host);
  free(http);
  free(path);
  free(escaped_q);

  return person;
}

bool client_add(Person *person)
{
  if (!person)
    return false;

  // Serialize person
  JSON_Value *root_value = json_value_init_object();
  char *json = person_to_json_string(root_value, person);
  json_value_free(root_value);

  // Check environment variable for updated server and port
  const char *env_server = g_getenv("PB_SERVER");
  const char *env_port = g_getenv("PB_PORT");

  if (env_server)
    server = env_server;
  if (env_port)
    port = env_port;

  // Construct the "Host:" HTTP header content (combine server and port)
  size_t host_size = strlen(server) + strlen(":") + strlen(port) + 1; // + 1 NUL
  char *server_host = g_malloc(host_size);
  g_snprintf(server_host, host_size, "%s:%s", server, port);

  char *http = http_request_wrap_message(
      HTTP_POST,
      "/add",
      (struct HttpHeaders){
          .host = server_host,
          .content_type = "application/json"},
      json);

  g_message("HTTP request: ===\n%s\n===", http);

  // Send it over socket
  // TODO
  bool result = false;

  g_message("%s: stub, not implemented!", __func__);

  // Free unused memory
  free(http);
  free(server_host);
  free(json);

  return result;
}

bool client_edit(const char *name, Person *person)
{
  if (!person)
    return false;

  // URL encode the resource path to edit
  char *escaped_name = g_uri_escape_string(name, NULL, TRUE);

  // Path to query
  size_t path_size = strlen("/") + strlen(escaped_name) + 1; // + 1 NUL
  char *path = g_malloc(path_size);
  g_snprintf(path, path_size, "/%s", escaped_name);

  // Serialize person
  JSON_Value *root_value = json_value_init_object();
  char *json = person_to_json_string(root_value, person);
  json_value_free(root_value);

  // Check environment variable for updated server and port
  const char *env_server = g_getenv("PB_SERVER");
  const char *env_port = g_getenv("PB_PORT");

  if (env_server)
    server = env_server;
  if (env_port)
    port = env_port;

  // Construct the "Host:" HTTP header content (combine server and port)
  size_t host_size = strlen(server) + strlen(":") + strlen(port) + 1; // + 1 NUL
  char *server_host = g_malloc(host_size);
  g_snprintf(server_host, host_size, "%s:%s", server, port);

  char *http = http_request_wrap_message(
      HTTP_POST,
      path,
      (struct HttpHeaders){
          .host = server_host,
          .content_type = "application/json"},
      json);

  g_message("HTTP request: ===\n%s\n===", http);

  // Send it over socket
  // TODO
  bool result = false;

  g_message("%s: stub, not implemented!", __func__);

  // Free unused memory
  free(http);
  free(server_host);
  free(json);
  free(path);
  free(escaped_name);

  return result;
}
