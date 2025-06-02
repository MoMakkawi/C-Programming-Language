#include "chat.h"

#include <stdio.h>
#include <string.h>

/**
 * Given a port, creates the sockaddr_in data associated with the server.
 * The address still needs to be specified
 */
struct sockaddr_in setupServer(uint16_t _port) {
    struct sockaddr_in retval;
    memset(&retval, 0, sizeof(retval)); // Ensure all fields are initialized to 0
    retval.sin_family = AF_INET;        // Set to IPv4
    retval.sin_port = htons(_port);     // Convert port to network byte order
    return retval;
}

int readMessage(int _connectedFd, struct Message* _message) {
    memset(_message, 0, sizeof(struct Message)); // Clear out message buffer before reading
    int flags = 0; // No special flags used with recv
    int bytesReceived =
        recv(_connectedFd, _message, sizeof(struct Message), flags);
    if (bytesReceived == -1) {
        return -1; // Error occurred while receiving
    }
    return 0;
}

/**
 * Given a socket and a message, sends it.
 */
int sendMessage(int _sockfd, struct Message* _message) {
    // Assumes the full Message struct is sent in one call
    if (send(_sockfd, _message, sizeof(struct Message), 0) == -1) {
        return -1; // Sending failed
    }
    return 0;
}
