#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chat.h"

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int settingUpClientSocket() {
    struct sockaddr_in serverAddress = setupServer(PORT);

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Error creating socket");
    }

    if (connect(sockfd, (struct sockaddr*)&serverAddress,
            sizeof(struct sockaddr_in))
        != 0) {
        error("Error connecting to server");
    }

    int on;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on))
        < 0) {
        error("Error setting reuse option\n");
    }

    // Set the IP address of the server (overwrites sin_addr)
    if (inet_aton(SERVER_ADDR, &serverAddress.sin_addr) == 0) {
        error("Error converting address");
    }

    return sockfd;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please specify a nickname\n");
    }

    printf("Welcome %s\n", argv[1]);

    struct Message outgoing;
    strcpy(outgoing.nickname, argv[1]); // Set user's nickname for outgoing messages

    int sockfd = settingUpClientSocket();

    struct Message incoming;
    struct pollfd fds[2];

    fds[0].fd = 0;             // Standard input (keyboard)
    fds[0].events = POLLIN;    // Monitor for input
    fds[1].fd = sockfd;        // Server socket
    fds[1].events = POLLIN;    // Monitor for incoming messages

    // Start sending and receiving messages
    while (1) {
        printf("%s> ", outgoing.nickname);
        fflush(stdout); // Ensure prompt is shown immediately

        int retpoll = poll(fds, 2, -1); // Wait for input from user or server
        if (retpoll < 0) {
            perror("poll failed");
            return -1;
        }
        if (retpoll == 0) {
            continue; // Shouldn't happen with -1 timeout, but safe to check
        }

        if (fds[0].revents == POLLIN) {
            fgets(outgoing.message, 1023, stdin); // Read user input
            outgoing.message[strcspn(outgoing.message, "\n")] = 0; // Remove newline

            if (sendMessage(sockfd, &outgoing) < 0) {
                perror("Error sending message");
                break;
            }
        } else if (fds[1].revents == POLLIN) {
            if (readMessage(fds[1].fd, &incoming) < 0) {
                perror("Error reading message");
                break;
            }
            // Print received message, overwriting current prompt
            printf("\r%s> %s\n", incoming.nickname, incoming.message);
        } else {
            printf("Unexcepted revent\n"); // Should not occur unless there's a poll error
            break;
        }
    }

    close(sockfd); // Clean up on exit
    return 0;
}
