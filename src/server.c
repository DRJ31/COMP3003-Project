#include <stdio.h>
#include "glib.h"
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

#define BUFSIZE 1024
#define IPBUFSIZE 128

void *client_handler(void *arg)
{
  char buf[BUFSIZE]; // String for received message
  int cfd = *(int *)arg;

  read(cfd, buf, sizeof(buf));
  // Read data
  strcat(buf, " 200 OK");
  write(cfd, buf, strlen(buf));
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
