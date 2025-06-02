# Dynamic Port Selection Design

## Current Implementation
The current implementation has the following limitations regarding port selection:

1. Port number (12345) is hardcoded as a macro in both server.c and client.c
2. Changing the port requires recompiling both applications
3. No flexibility for environments where the default port might be unavailable

## Proposed Solution

### 1. Server-Side Changes
Modify the server to accept a port number as a command-line argument:

```c
// In server.c
int main(int argc, char** argv) {
    uint16_t port = 12345;  // Default port
    
    // Parse command-line arguments
    if (argc > 1) {
        port = (uint16_t)atoi(argv[1]);
        if (port <= 0) {
            fprintf(stderr, "Invalid port number. Using default port %d\n", 12345);
            port = 12345;
        }
    }
    
    struct Server server = createServer(port);  // Pass port to createServer
    printf("Server started on port %d\n", port);
    runServer(&server);
    return 0;
}
```

Update the createServer function to accept a port parameter:

```c
struct Server createServer(uint16_t port) {
    // ... existing socket creation code ...
    
    // Bind the socket to the address with the specified port
    struct sockaddr_in serverAddress = setupServer(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // ... rest of function ...
}
```

### 2. Client-Side Changes
Modify the client to accept a server address and port as command-line arguments:

```c
// In client.c
int main(int argc, char** argv) {
    char* serverAddr = "127.0.0.1";  // Default server address
    uint16_t port = 12345;           // Default port
    
    // Check for nickname
    if (argc < 2) {
        printf("Usage: %s <nickname> [server_address] [port]\n", argv[0]);
        return 1;
    }
    
    // Parse optional server address
    if (argc > 2) {
        serverAddr = argv[2];
    }
    
    // Parse optional port
    if (argc > 3) {
        port = (uint16_t)atoi(argv[3]);
        if (port <= 0) {
            fprintf(stderr, "Invalid port number. Using default port %d\n", 12345);
            port = 12345;
        }
    }
    
    printf("Connecting to %s:%d as %s\n", serverAddr, port, argv[1]);
    
    // ... rest of client setup ...
    
    int sockfd = settingUpClientSocket(serverAddr, port);
    
    // ... rest of client code ...
}
```

Update the settingUpClientSocket function to accept address and port parameters:

```c
int settingUpClientSocket(const char* serverAddr, uint16_t port) {
    struct sockaddr_in serverAddress = setupServer(port);
    int sockfd;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Error creating socket");
    }
    
    // Fill in the server address structure
    if (inet_aton(serverAddr, &serverAddress.sin_addr) == 0) {
        error("Error converting address");
    }
    
    if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in)) != 0) {
        error("Error connecting to server");
    }
    
    // ... rest of function ...
    
    return sockfd;
}
```

### 3. Update Function Prototypes
Update the function prototypes in header files:

```c
// In server.h
struct Server createServer(uint16_t port);

// In client.c (or a new client.h if created)
int settingUpClientSocket(const char* serverAddr, uint16_t port);
```

## Benefits of This Design

1. **Flexibility**: Users can specify any available port at runtime
2. **Compatibility**: Default values ensure backward compatibility
3. **Usability**: Clear error messages guide users when invalid values are provided
4. **Extensibility**: The client can now connect to servers on different machines

## Implementation Considerations

1. **Input Validation**: Ensure port numbers are within valid range (1-65535)
2. **Error Handling**: Provide clear error messages for invalid inputs
3. **Documentation**: Update usage instructions to reflect new command-line options
4. **Testing**: Test with various port numbers, including edge cases
