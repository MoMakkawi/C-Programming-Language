#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>  // For timestamps

#include "chat.h"
#include "server.h"
#include "message_store.h"  // Handles persistent message logging

#define PORT 12345  // Default port the server listens on

void error(const char* msg) {
    perror(msg);
    exit(1);
}

// Initializes the server socket, binds it, and sets it up for polling
struct Server createServer() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        error("Error creating socket");
    }

    struct Server retval;
    retval.nbClients = 0;
    memset(&retval.fds[0], 0, sizeof(struct pollfd));
    retval.fds[0].fd = fd;
    retval.fds[0].events = POLLIN;

    // Set up server address and bind to the specified port
    struct sockaddr_in serverAddress = setupServer(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(retval.fds[0].fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        error("Error binding");
    }

    int on = 1;
    if (setsockopt(retval.fds[0].fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        error("Error setting reuse option");
    }

    if (ioctl(retval.fds[0].fd, FIONBIO, (char*)&on) < 0) {
        error("Error setting non block option");
    }

    if (listen(retval.fds[0].fd, 32) < 0) {
        error("Error setting listen option");
    }

    return retval;
}

// Accepts new incoming client connections and adds them to the poll set
int acceptNewClients(struct Server* _server) {
    if (_server == NULL) {
        return -2;
    }

    int newFd;
    do {
        newFd = accept(_server->fds[0].fd, NULL, NULL);
        if (newFd < 0) {
            if (errno != EWOULDBLOCK) {
                perror("accept() failed");
                return -1;
            }
            break;
        }

        _server->nbClients++;
        memset(&_server->fds[_server->nbClients], 0, sizeof(struct pollfd));
        _server->fds[_server->nbClients].fd = newFd;
        _server->fds[_server->nbClients].events = POLLIN;

        printf("New client connected: %d\n", newFd);
    } while (newFd != -1 && _server->nbClients < 2);  // Accept at most 2 clients

    return 0;
}

// Handles receiving a message from one client and broadcasting it to all others
int receiveAndBroadcastMessage(struct Server* _server, int _sendingClientFd) {
    if (_server == NULL) {
        return -3;
    }

    struct Message message;
    int read = readMessage(_sendingClientFd, &message);
    if (read < 0) {
        return -1;
    }

    message.timestamp = time(NULL);  // Add timestamp

    // Save to persistent store
    if (saveMessage(&message) < 0) {
        printf("Warning: Failed to save message to history\n");
    }

    // Broadcast to all connected clients except the sender
    for (int j = 1; j <= _server->nbClients; ++j) {
        if (_server->fds[j].fd == _sendingClientFd) {
            continue;
        }

        int sent = sendMessage(_server->fds[j].fd, &message);
        if (sent < 0) {
            _server->fds[j].fd = -1;  // Mark socket for cleanup
            return -2;
        }
    }

    // Display message on server console
    char timeStr[64];
    struct tm* timeinfo = localtime(&message.timestamp);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("[%s] %s> %s\n", timeStr, message.nickname, message.message);

    return 0;
}

// Cleans up pollfd list by removing disconnected clients (fd = -1)
void compactDescriptor(struct Server* _server) {
    if (_server == NULL) {
        return;
    }

    for (int i = 1; i <= _server->nbClients; i++) {
        if (_server->fds[i].fd == -1) {
            for (int j = i; j < _server->nbClients; j++) {
                _server->fds[j] = _server->fds[j + 1];
            }
            i--;
            _server->nbClients--;
        }
    }
}

// The main event loop of the server, handling new connections and client messages
void runServer(struct Server* _server) {
    if (_server == NULL) {
        return;
    }

    while (1) {
        printf("Current nb clients: %d\n", _server->nbClients);
        int retpoll = poll(_server->fds, _server->nbClients + 1, -1);
        if (retpoll < 0) {
            perror("Call to poll failed");
            break;
        }
        if (retpoll == 0) {
            continue;
        }

        printf("Checking %d fds\n", retpoll);

        // Handle new client connections
        if (_server->fds[0].revents == POLLIN) {
            if (acceptNewClients(_server) == -1) {
                perror("Error accepting new clients");
                break;
            }
        }

        // Handle messages from clients
        for (int i = 1; i <= _server->nbClients; i++) {
            if (_server->fds[i].revents == 0) {
                continue;
            }

            if (_server->fds[i].revents != POLLIN) {
                _server->fds[i].fd = -1;
                break;
            }

            int broadcast = receiveAndBroadcastMessage(_server, _server->fds[i].fd);
            if (broadcast == -1) {
                perror("Error reading message");
                _server->fds[i].fd = -1;
                break;
            }
        }

        compactDescriptor(_server);
    }

    // Cleanup: close all file descriptors
    for (int i = 0; i <= _server->nbClients; ++i) {
        if (_server->fds[i].fd >= 0) {
            close(_server->fds[i].fd);
        }
    }

    // Close file used for storing messages
    closeMessageStore();
}

// Entry point: sets up server and optionally prints last messages from storage
int main() {
    if (initMessageStore("chat_history.dat") != 0) {
        fprintf(stderr, "Warning: Failed to initialize message store\n");
    } else {
        printf("Message store initialized successfully\n");

        // Load and print last 10 messages on startup (sorted by time, descending)
        struct Message lastMessages[10];
        int numLoaded = loadMessages(lastMessages, 10, 1, 0);

        if (numLoaded > 0) {
            printf("Last %d messages:\n", numLoaded);
            printf("--------------------\n");
            for (int i = 0; i < numLoaded; i++) {
                char timeStr[64];
                struct tm* timeinfo = localtime(&lastMessages[i].timestamp);
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);

                printf("[%s] %s: %s\n",
                       timeStr,
                       lastMessages[i].nickname,
                       lastMessages[i].message);
            }
            printf("--------------------\n");
        }
    }

    struct Server server = createServer();
    printf("Server started on port %d\n", PORT);
    runServer(&server);

    return 0;
}
