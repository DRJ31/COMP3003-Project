#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUFSIZE 1024

char *msg_transfer(char *host, uint16_t port, char *msg) {
    char buf[BUFSIZE] = {0}; // Receive respond from server
    char *result; // Store result

    // Create file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket error");
        exit(-1);
    }

    // Connect to server
    struct sockaddr_in c_addr;
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(port); // Modify server port here
    inet_pton(AF_INET, host, &c_addr.sin_addr.s_addr); // Modify server address here

    int ret = connect(fd, (struct sockaddr*)&c_addr, sizeof(c_addr));
    if (ret == -1) {
        perror("Connect error");
        exit(-1);
    }

    // Send message
    write(fd, msg, strlen(msg)); // Send data to server
    // Receive message
    ssize_t len = read(fd, buf, sizeof(buf));
    if (len == -1) {
        perror("Read error");
        exit(-1);
    }
    result = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
    strcpy(result, buf);
    close(fd);
    return result;
}
