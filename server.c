#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define BUFSIZE 1024
#define IPBUFSIZE 128

void *client_thread(void *arg) {
    char buf[BUFSIZE]; // String for received message
    int cfd = *(int *)arg;
    
    while (1) {
        // Read data
        memset(buf, 0, sizeof(buf)); // Initialize data string
        read(cfd, buf, sizeof(buf)); // Read data received
        if (strcmp(buf, "exit") == 0)
            break;
        printf("Msg: %s\n", buf);
        strcat(buf, " 200 OK");
        write(cfd, buf, strlen(buf));
    }
    pthread_exit(NULL);
}

int main(void) {
    // Create new socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0); // Server file descriptor
    int cfd; // Client file descriptor
    struct sockaddr_in serv_addr; // Server address information
    struct sockaddr_in clien_addr; // Client address information
    char ipbuf[IPBUFSIZE]; // String for ip information
    
    // Binding
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081); // Binding port
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Binding address
    bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    // Listen to port
    listen(sfd, 64);
    
    // Handle clients
    while (1) {
        pthread_t thread_id;
        socklen_t clien_len = sizeof(clien_addr);
        cfd = accept(sfd, (struct sockaddr*)&clien_addr, &clien_len);
        printf("Client IP: %s\tPort: %d\n",
               inet_ntop(AF_INET, &clien_addr.sin_addr.s_addr, ipbuf, sizeof(ipbuf)),
               ntohs(clien_addr.sin_port));
        pthread_create(&thread_id, NULL, client_thread, &cfd);
    }
    return 0;
}
