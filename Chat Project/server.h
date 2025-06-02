#ifndef SERVER_H
#define SERVER_H

#include <poll.h>

/**
 * The Server structure stores the file descriptors of:
 *  - the listening socket
 *  - up to 2 client sockets
 * It uses the `poll` mechanism to monitor events on each socket.
 */
struct Server {
    struct pollfd fds[3];   ///< fds[0] is the listening socket; fds[1..2] are client sockets
    int nbClients;          ///< Number of clients currently connected (max 2)
};

/**
 * Accept new client connections if there are fewer than 2 clients connected.
 *
 * @param _server Pointer to the Server struct.
 * @return 0 on success,
 *        -1 on socket error (accept failed),
 *        -2 if _server is NULL.
 */
int acceptNewClients(struct Server* _server);

/**
 * Receive a message from the given client socket and broadcast it to other connected clients.
 *
 * @param _server Pointer to the Server struct.
 * @param _sendingClientFd File descriptor of the client sending the message.
 * @return 0 on success,
 *        -1 if the message could not be read (sender fd is set to -1),
 *        -2 if the message could not be sent to any recipient (receiver fd is set to -1),
 *        -3 if _server is NULL.
 */
int receiveAndBroadcastMessage(struct Server* _server, int _sendingClientFd);

/**
 * Compact the pollfd array by removing disconnected clients (fds marked as -1).
 *
 * This function updates the pollfd array in-place and decrements nbClients accordingly.
 *
 * @param _server Pointer to the Server struct.
 */
void compactDescriptor(struct Server* _server);

#endif
