# Multi-Client Support Design

## Current Implementation
The current server implementation has the following limitations:

1. Fixed array of 3 file descriptors in the Server structure (1 for listening socket + 2 for clients)
2. Hard limit of 2 clients enforced in the acceptNewClients function
3. Client management logic assumes a maximum of 2 clients

## Proposed Solution

### 1. Dynamic Client Management
Replace the fixed-size array with a dynamically allocated array that can grow as needed:

```c
struct Server {
    struct pollfd* fds;         // Dynamically allocated array for file descriptors
    int capacity;               // Current capacity of the fds array
    int nbClients;              // Current number of clients connected
};
```

### 2. Server Initialization
Initialize the server with a reasonable starting capacity that can be expanded:

```c
struct Server createServer() {
    // ... existing socket creation code ...
    
    struct Server retval;
    retval.nbClients = 0;
    retval.capacity = 10;  // Start with capacity for 10 clients (9 clients + 1 listening socket)
    retval.fds = malloc(retval.capacity * sizeof(struct pollfd));
    
    if (retval.fds == NULL) {
        error("Memory allocation failed");
    }
    
    memset(retval.fds, 0, retval.capacity * sizeof(struct pollfd));
    retval.fds[0].fd = fd;  // Listening socket
    retval.fds[0].events = POLLIN;
    
    // ... rest of initialization ...
    
    return retval;
}
```

### 3. Dynamic Array Resizing
Add a function to resize the array when needed:

```c
int resizeServerCapacity(struct Server* _server, int newCapacity) {
    if (_server == NULL || newCapacity <= _server->capacity) {
        return -1;
    }
    
    struct pollfd* newFds = realloc(_server->fds, newCapacity * sizeof(struct pollfd));
    if (newFds == NULL) {
        return -1;  // Memory allocation failed
    }
    
    _server->fds = newFds;
    _server->capacity = newCapacity;
    return 0;
}
```

### 4. Modified Client Acceptance
Update the acceptNewClients function to accept any number of clients:

```c
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
        
        // Check if we need to resize the array
        if (_server->nbClients + 1 >= _server->capacity) {
            if (resizeServerCapacity(_server, _server->capacity * 2) < 0) {
                close(newFd);  // Close the connection if we can't handle it
                return -3;
            }
        }
        
        _server->nbClients++;
        memset(&_server->fds[_server->nbClients], 0, sizeof(struct pollfd));
        _server->fds[_server->nbClients].fd = newFd;
        _server->fds[_server->nbClients].events = POLLIN;
        printf("New client connected: %d\n", _server->fds[_server->nbClients].fd);
    } while (newFd != -1);  // Accept as many clients as possible
    
    return 0;
}
```

### 5. Server Cleanup
Add proper cleanup to free allocated memory:

```c
void cleanupServer(struct Server* _server) {
    if (_server == NULL) {
        return;
    }
    
    // Close all client connections
    for (int i = 0; i <= _server->nbClients; i++) {
        if (_server->fds[i].fd >= 0) {
            close(_server->fds[i].fd);
        }
    }
    
    // Free the dynamically allocated array
    free(_server->fds);
    _server->fds = NULL;
    _server->capacity = 0;
    _server->nbClients = 0;
}
```

### 6. Updated Server Loop
Modify the runServer function to handle the dynamic array:

```c
void runServer(struct Server* _server) {
    if (_server == NULL) {
        return;
    }
    
    while (1) {
        printf("Current nb clients: %d\n", _server->nbClients);
        int retpoll = poll(_server->fds, _server->nbClients + 1, -1);
        
        // ... existing error handling ...
        
        // Check for new connections on the listening socket
        if (_server->fds[0].revents == POLLIN) {
            if (acceptNewClients(_server) == -1) {
                perror("Error accepting new clients");
                break;
            }
        }
        
        // Check client sockets (now with dynamic number of clients)
        for (int i = 1; i <= _server->nbClients; i++) {
            // ... existing client handling logic ...
        }
        
        compactDescriptor(_server);
    }
    
    cleanupServer(_server);
}
```

## Benefits of This Design

1. **Scalability**: The server can now handle an arbitrary number of clients, limited only by system resources.
2. **Efficiency**: The dynamic array grows only when needed, avoiding unnecessary memory allocation.
3. **Robustness**: Proper error handling and cleanup prevent memory leaks.
4. **Minimal Changes**: The core logic of the server remains largely unchanged, reducing the risk of introducing bugs.

## Implementation Considerations

1. **Memory Management**: Careful attention to memory allocation, reallocation, and freeing is essential.
2. **Error Handling**: All memory operations must be checked for failure.
3. **Performance**: The reallocation strategy (doubling capacity) provides amortized O(1) insertion time.
4. **Testing**: Thorough testing with varying numbers of clients will be necessary.
