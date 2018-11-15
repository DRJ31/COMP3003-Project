#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUFSIZE 1024

int main(void) {
    // Create
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket error");
        exit(-1);
    }
    
    // Connect
    struct sockaddr_in c_addr;
    bzero(&c_addr, sizeof(c_addr));
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(8081);
    inet_pton(AF_INET, "127.0.0.1", &c_addr.sin_addr.s_addr);
    
    int ret = connect(fd, (struct sockaddr*)&c_addr, sizeof(c_addr));
    if (ret == -1) {
        perror("Connect error");
        exit(-1);
    }
    
    while (1) {
        // Send message
        char buf[BUFSIZE] = {0};
        puts("Input a string: ");
        // Exit the program by Ctrl-D or enter "exit"
        if (scanf("%s", buf) == EOF || strcmp(buf, "exit") == 0) {
            write(fd, "exit", 4);
            break;
        }
        write(fd, buf, strlen(buf));
        
        // Receive message
        int len = read(fd, buf, sizeof(buf));
        if (len == -1) {
            perror("Read error");
            exit(-1);
        }
        puts(buf);
    }
    close(fd);
    return 0;
}
