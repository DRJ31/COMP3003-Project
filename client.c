#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUFSIZE 1024

int main(void) {
    // Create file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket error");
        exit(-1);
    }

    // Connect to server
    struct sockaddr_in c_addr;
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(8081); // Modify server port here
    inet_pton(AF_INET, "127.0.0.1", &c_addr.sin_addr.s_addr); // Modify server address here

    int ret = connect(fd, (struct sockaddr*)&c_addr, sizeof(c_addr));
    if (ret == -1) {
        perror("Connect error");
        exit(-1);
    }

    char buf[BUFSIZE] = {0};
    write(fd, "tcp", 3); // Tell server it is tcp client
    read(fd, buf, sizeof(buf)); // Receive return message
    puts(buf);
    while (1) {
        // Send message
        memset(buf, 0, sizeof(buf)); // Initialize string
        puts("Input a string: ");
        // Exit the program by Ctrl-D or enter "exit"
        if (scanf("%s", buf) == EOF || strcmp(buf, "exit") == 0) {
            write(fd, "exit", 4);
            break;
        }
        write(fd, buf, strlen(buf)); // Send data to server

        // Receive message
        ssize_t len = read(fd, buf, sizeof(buf));
        if (len == -1) {
            perror("Read error");
            exit(-1);
        }
        puts(buf);
    }
    close(fd);
    return 0;
}
