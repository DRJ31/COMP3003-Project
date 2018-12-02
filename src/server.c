#include <stdio.h>
#include "glib.h"
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "json.h"
#include "http.h"

#define BUFSIZE 1024
#define IPBUFSIZE 128

char **path_extract(char *path, int length) 
{
  int count = 0, index = 0, start = 1;
  char **path_value = (char**)malloc(sizeof(char*) * length);
  for (int i = 0; i < length; i++) {
    path_value[i] = (char *)malloc(sizeof(char) * BUFSIZE);
  }
  for (unsigned int i = 1; i < strlen(path); i++) {
    if (path[i] == '/') {
      strncpy(path_value[index], path + start, count);
      path_value[index][count] = '\0';
      count = 0;
      puts(path_value[index]);
      index++;
      start = i + 1;
      continue;
    }
    count++;
  }
  strncpy(path_value[index], path + start, count);
  path_value[index][count] = '\0';
  return path_value;
}

void *client_handler(void *arg)
{
  char buf[BUFSIZE]; // String for received message
  int cfd = *(int *)arg;
  Person *person = NULL;

  read(cfd, buf, sizeof(buf));
  enum HttpMethod method = http_extract_request_method(buf);
  switch (method) {
    case HTTP_GET: { 
      int size[4];
      puts(buf);
      char **keyword = path_extract(http_extract_request_path(buf), 1);
      JSON_Value *root_value = json_value_init_object();
      puts(keyword[0]);
      person = get_person(keyword[0], size);
      char *person_json;
      char *response;
      if (person == NULL) {
        response = http_response_wrap_message(
          HTTP_STATUS_NOT_FOUND,
          (struct HttpHeaders) {
            .host = NULL,
            .accept = NULL,
            .content_type = "application/json",
            .authorization = NULL,
          },
          NULL
        );
      }
      else {
        person_json = person_to_json_string(root_value, person);
        response = http_response_wrap_message(
          HTTP_STATUS_OK,
          (struct HttpHeaders) {
            .host = NULL,
            .accept = NULL,
            .content_type = "application/json",
            .authorization = NULL,
          },
          person_json
        );
      }
      puts(response);
      memset(buf, 0, BUFSIZE);
      strcpy(buf, response);
      free(response);
      free(root_value);
      free(keyword);
      // free(person_json);
      break;
    }
    case HTTP_POST: { 
      person = json_string_to_person(http_extract_payload(buf));
      int status = insert_info(person);
      memset(buf, 0, BUFSIZE);
      if (status)
        strcpy(buf, "Success");
      else
        strcpy(buf, "Failed");
      break;
    }
    case HTTP_PUT: {
      int status = check_pass("lmy441900", "madoka");
      memset(buf, 0, BUFSIZE);
      if (status)
        strcpy(buf, "Success");
      else
        strcpy(buf, "Failed");
      break;
    }
    default:
      break;
  }
  write(cfd, buf, strlen(buf));
  free(person);
  close(cfd);
  puts("Thread closed");
  pthread_exit(NULL);
}

void server_start(int *argc, char ***argv)
{
  if (*argc < 4)
  {
    printf("Usage: %s server <address> <port>\n", *argv[0]);
    return;
  }
  int port = atoi((*argv)[3]);
  // Create new socket
  int sfd = socket(AF_INET, SOCK_STREAM, 0); // Server file descriptor
  int cfd;                                   // Client file descriptor
  struct sockaddr_in serv_addr;              // Server address information
  struct sockaddr_in clien_addr;             // Client address information
  char ipbuf[IPBUFSIZE];                     // String for ip information

  // Binding
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port); // Binding port
  inet_pton(AF_INET, (*argv)[2], &serv_addr.sin_addr.s_addr);
  //  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Binding address
  bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  // Listen to port
  listen(sfd, 64);

  printf("Running on port: %d\n", ntohs(serv_addr.sin_port));

  // Handle clients
  while (1)
  {
    pthread_t thread_id;
    socklen_t clien_len = sizeof(clien_addr);
    cfd = accept(sfd, (struct sockaddr *)&clien_addr, &clien_len);
    printf("Client IP: %s\tPort: %d\n",
           inet_ntop(AF_INET, &clien_addr.sin_addr.s_addr, ipbuf, sizeof(ipbuf)),
           ntohs(clien_addr.sin_port));
    pthread_create(&thread_id, NULL, client_handler, &cfd);
  }
  // g_message("%s: stub, not implemented!", __func__);s
}
