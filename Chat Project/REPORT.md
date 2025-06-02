# Chat Application Extension - Technical Report

## Overview

This report documents the extension of a basic chat application to address three key limitations in the original implementation:

1. Limited client support (maximum of 2 clients)
2. Fixed server port requiring recompilation to change
3. Lack of message history persistence

The project successfully implements solutions for all three limitations while maintaining compatibility with the original application's core functionality.

## Original Application Analysis

The original chat application consisted of a server and client components with the following limitations:

### Client Limitation
The server used a fixed-size array of file descriptors with a hardcoded limit of 2 clients:
```c
struct Server {
    struct pollfd fds[3];    // Array storing the listening socket + the client socket
    int nbClients;           // Current number of clients connected to the server
};
```

The `acceptNewClients` function enforced this limitation by only accepting connections until two clients were connected.

### Port Configuration
The port number was defined as a macro in both server.c and client.c:
```c
#define PORT 12345
```

This required recompiling the application to change the port number.

### Message Handling
Messages were only forwarded between clients without any persistence. The Message structure contained:
```c
struct Message {
    char nickname[NAME_LENGTH];  // Nickname of the user that sent the message
    char message[BUFFER_LENGTH]; // Message sent
    int flags;                   // Reserved for Digital manufacturing
};
```

## Implementation Solutions

### 1. Multi-Client Support

#### Design Approach
The fixed-size array was replaced with a dynamically allocated array that can grow as needed:

```c
struct Server {
    struct pollfd* fds;     // Dynamically allocated array for file descriptors
    int capacity;           // Current capacity of the fds array
    int nbClients;          // Current number of clients connected
};
```

#### Key Implementation Details
- **Dynamic Memory Allocation**: The server now allocates memory for client connections at startup
- **Automatic Resizing**: When capacity is reached, the array is automatically resized
- **Memory Management**: Proper allocation, reallocation, and cleanup prevent memory leaks
- **Error Handling**: Robust error checking for memory operations

#### Benefits
- The server can now handle an arbitrary number of clients, limited only by system resources
- The implementation is efficient, only growing the array when needed
- Memory usage scales with the actual number of connected clients

### 2. Dynamic Port Selection

#### Design Approach
Both server and client were modified to accept port numbers as command-line arguments:

```c
// Server
int main(int argc, char** argv) {
    uint16_t port = DEFAULT_PORT;  // Default port
    
    // Parse command-line arguments
    if (argc > 1) {
        port = (uint16_t)atoi(argv[1]);
        // Validation and error handling...
    }
    // ...
}

// Client
int main(int argc, char** argv) {
    // ...
    // Parse optional port
    if (argc > 3) {
        port = (uint16_t)atoi(argv[3]);
        // Validation and error handling...
    }
    // ...
}
```

#### Key Implementation Details
- **Command-Line Arguments**: Both applications now parse command-line arguments for port configuration
- **Default Values**: Default port values ensure backward compatibility
- **Input Validation**: Port numbers are validated to ensure they are within valid range
- **Error Handling**: Clear error messages guide users when invalid values are provided

#### Benefits
- Users can specify any available port at runtime
- No recompilation needed to change ports
- Backward compatibility is maintained with default values
- Clear error messages improve usability

### 3. Message History Storage

#### Design Approach
A file-based message storage system was implemented with an extended Message structure:

```c
struct Message {
    char nickname[NAME_LENGTH];  // Nickname of the user that sent the message
    char message[BUFFER_LENGTH]; // Message sent
    time_t timestamp;           // Time when the message was sent
    int flags;                  // Reserved for Digital manufacturing
};
```

#### Key Implementation Details
- **Timestamp Addition**: Messages now include a timestamp when they are sent
- **File-Based Storage**: Messages are saved to a binary file for persistence
- **Server Integration**: The server displays the last 10 messages on startup
- **History Viewer**: A separate application allows viewing and sorting message history

#### Benefits
- Messages are preserved between server restarts
- Timestamps provide chronological context for messages
- The history viewer offers flexible sorting and filtering options
- The implementation is efficient with minimal overhead

## Testing and Validation

All features were thoroughly tested to ensure they meet the requirements:

### Multi-Client Support
- Successfully tested with multiple simultaneous client connections (3+ clients)
- Verified that the server properly handles client connections and disconnections
- Confirmed that messages are correctly broadcast to all connected clients

### Dynamic Port Selection
- Tested server startup with various port numbers
- Verified client connections to custom ports
- Confirmed backward compatibility with default port values
- Validated error handling for invalid port numbers

### Message History Storage
- Verified message persistence across server restarts
- Confirmed timestamp accuracy in stored messages
- Tested the history viewer with various sorting and filtering options
- Validated that the server correctly displays the last 10 messages on startup

## Conclusion

The extended chat application successfully addresses all three limitations of the original implementation:

1. The server now supports an unlimited number of clients
2. The port can be changed without recompiling the application
3. Messages are saved with timestamps and can be viewed with a dedicated utility

The implementation maintains backward compatibility while adding significant new functionality. The code is well-structured, properly documented, and includes robust error handling.

## Future Improvements

Potential future enhancements could include:

- Adding user authentication and access control
- Implementing message encryption for security
- Creating a graphical user interface
- Supporting message categories or channels
- Adding support for file transfers or multimedia messages
