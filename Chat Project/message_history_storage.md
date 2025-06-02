# Message History Storage Design

## Current Implementation
The current implementation has the following limitations regarding message history:

1. Messages are only forwarded between clients and not stored
2. The Message structure doesn't include a timestamp
3. There's no way to view message history when the server restarts
4. No functionality exists to sort or filter messages

## Proposed Solution

### 1. Enhanced Message Structure
Extend the Message structure to include timestamp information:

```c
// In chat.h
struct Message {
    char nickname[NAME_LENGTH];     // Nickname of the user that sent the message
    char message[BUFFER_LENGTH];    // Message sent
    time_t timestamp;               // Time when the message was sent
    int flags;                      // Reserved for Digital manufacturing
};
```

### 2. Message Storage
Create a message storage system using a file-based approach:

```c
// In a new file: message_store.h
#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include "chat.h"

/**
 * Initialize the message store
 * @param filename The file to use for message storage
 * @return 0 on success, -1 on failure
 */
int initMessageStore(const char* filename);

/**
 * Save a message to the message store
 * @param message The message to save
 * @return 0 on success, -1 on failure
 */
int saveMessage(const struct Message* message);

/**
 * Load the last N messages from the message store
 * @param messages Array to store the loaded messages
 * @param maxMessages Maximum number of messages to load
 * @param sortByTime If true, sort by timestamp; if false, sort by nickname
 * @param ascending If true, sort in ascending order; if false, sort in descending order
 * @return Number of messages loaded, or -1 on failure
 */
int loadMessages(struct Message* messages, int maxMessages, int sortByTime, int ascending);

/**
 * Close the message store
 */
void closeMessageStore();

#endif
```

Implementation in message_store.c:

```c
#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE* messageFile = NULL;
static char currentFilename[256] = {0};

int initMessageStore(const char* filename) {
    if (messageFile != NULL) {
        fclose(messageFile);
    }
    
    strncpy(currentFilename, filename, sizeof(currentFilename) - 1);
    
    // Open file in append mode for writing
    messageFile = fopen(filename, "a+");
    if (messageFile == NULL) {
        perror("Failed to open message store file");
        return -1;
    }
    
    return 0;
}

int saveMessage(const struct Message* message) {
    if (messageFile == NULL || message == NULL) {
        return -1;
    }
    
    // Write the message to the file
    size_t written = fwrite(message, sizeof(struct Message), 1, messageFile);
    fflush(messageFile);  // Ensure it's written to disk
    
    return (written == 1) ? 0 : -1;
}

// Comparison function for sorting by time
static int compareByTime(const void* a, const void* b) {
    const struct Message* msgA = (const struct Message*)a;
    const struct Message* msgB = (const struct Message*)b;
    
    if (msgA->timestamp < msgB->timestamp) return -1;
    if (msgA->timestamp > msgB->timestamp) return 1;
    return 0;
}

// Comparison function for sorting by nickname
static int compareByNickname(const void* a, const void* b) {
    const struct Message* msgA = (const struct Message*)a;
    const struct Message* msgB = (const struct Message*)b;
    
    return strcmp(msgA->nickname, msgB->nickname);
}

int loadMessages(struct Message* messages, int maxMessages, int sortByTime, int ascending) {
    if (messages == NULL || maxMessages <= 0) {
        return -1;
    }
    
    // If the file isn't open, try to open it for reading
    if (messageFile == NULL) {
        messageFile = fopen(currentFilename, "r");
        if (messageFile == NULL) {
            perror("Failed to open message store file for reading");
            return -1;
        }
    }
    
    // Seek to the beginning of the file
    rewind(messageFile);
    
    // Read all messages into a temporary buffer
    struct Message* allMessages = NULL;
    int numMessages = 0;
    int capacity = 100;  // Initial capacity
    
    allMessages = malloc(capacity * sizeof(struct Message));
    if (allMessages == NULL) {
        return -1;
    }
    
    while (fread(&allMessages[numMessages], sizeof(struct Message), 1, messageFile) == 1) {
        numMessages++;
        
        // Resize if needed
        if (numMessages >= capacity) {
            capacity *= 2;
            struct Message* newBuffer = realloc(allMessages, capacity * sizeof(struct Message));
            if (newBuffer == NULL) {
                free(allMessages);
                return -1;
            }
            allMessages = newBuffer;
        }
    }
    
    // Sort the messages
    if (sortByTime) {
        qsort(allMessages, numMessages, sizeof(struct Message), compareByTime);
    } else {
        qsort(allMessages, numMessages, sizeof(struct Message), compareByNickname);
    }
    
    // Reverse the order if descending
    if (!ascending) {
        for (int i = 0; i < numMessages / 2; i++) {
            struct Message temp = allMessages[i];
            allMessages[i] = allMessages[numMessages - 1 - i];
            allMessages[numMessages - 1 - i] = temp;
        }
    }
    
    // Copy the messages to the output buffer (up to maxMessages)
    int messagesToCopy = (numMessages < maxMessages) ? numMessages : maxMessages;
    memcpy(messages, allMessages, messagesToCopy * sizeof(struct Message));
    
    free(allMessages);
    return messagesToCopy;
}

void closeMessageStore() {
    if (messageFile != NULL) {
        fclose(messageFile);
        messageFile = NULL;
    }
}
```

### 3. Server Integration
Modify the server to use the message store:

```c
// In server.c
#include "message_store.h"

// In main function
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
    
    // Initialize message store
    if (initMessageStore("chat_history.dat") != 0) {
        fprintf(stderr, "Warning: Failed to initialize message store\n");
    } else {
        // Display last 10 messages
        struct Message lastMessages[10];
        int numLoaded = loadMessages(lastMessages, 10, 1, 0);  // Sort by time, descending
        
        if (numLoaded > 0) {
            printf("Last %d messages:\n", numLoaded);
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
    
    struct Server server = createServer(port);
    printf("Server started on port %d\n", port);
    runServer(&server);
    
    closeMessageStore();
    return 0;
}

// In receiveAndBroadcastMessage function
int receiveAndBroadcastMessage(struct Server* _server, int _sendingClientFd) {
    if (_server == NULL) {
        return -3;
    }
    
    struct Message message;
    int read = readMessage(_sendingClientFd, &message);
    if (read < 0) {
        return -1;
    }
    
    // Add timestamp to the message
    message.timestamp = time(NULL);
    
    // Save the message to history
    saveMessage(&message);
    
    // Send message to all other clients
    for (int j = 1; j <= _server->nbClients; j++) {
        if (_server->fds[j].fd == _sendingClientFd) {
            continue;
        }
        int sent = sendMessage(_server->fds[j].fd, &message);
        if (sent < 0) {
            _server->fds[j].fd = -1;
            return -2;
        }
    }
    
    // Print the message with timestamp
    char timeStr[64];
    struct tm* timeinfo = localtime(&message.timestamp);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("[%s] %s> %s\n", timeStr, message.nickname, message.message);
    return 0;
}
```

### 4. Message History Viewer Application
Create a new application to view message history:

```c
// In history_viewer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chat.h"
#include "message_store.h"

void printUsage(const char* programName) {
    printf("Usage: %s [options]\n", programName);
    printf("Options:\n");
    printf("  -f <filename>   Specify message history file (default: chat_history.dat)\n");
    printf("  -n <number>     Number of messages to display (default: all)\n");
    printf("  -t              Sort by timestamp (default)\n");
    printf("  -u              Sort by username/nickname\n");
    printf("  -a              Sort in ascending order (default)\n");
    printf("  -d              Sort in descending order\n");
    printf("  -h              Display this help message\n");
}

int main(int argc, char** argv) {
    char filename[256] = "chat_history.dat";
    int maxMessages = -1;  // -1 means all messages
    int sortByTime = 1;    // Default: sort by time
    int ascending = 1;     // Default: ascending order
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strncpy(filename, argv[++i], sizeof(filename) - 1);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            maxMessages = atoi(argv[++i]);
            if (maxMessages <= 0) {
                fprintf(stderr, "Invalid number of messages\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            sortByTime = 1;
        } else if (strcmp(argv[i], "-u") == 0) {
            sortByTime = 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            ascending = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            ascending = 0;
        } else if (strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Initialize message store
    if (initMessageStore(filename) != 0) {
        fprintf(stderr, "Failed to open message history file: %s\n", filename);
        return 1;
    }
    
    // Determine how many messages to load
    if (maxMessages <= 0) {
        // Count total messages in the file
        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
            fprintf(stderr, "Failed to open message history file: %s\n", filename);
            return 1;
        }
        
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        maxMessages = fileSize / sizeof(struct Message);
        fclose(file);
        
        if (maxMessages <= 0) {
            printf("No messages in history\n");
            return 0;
        }
    }
    
    // Allocate memory for messages
    struct Message* messages = malloc(maxMessages * sizeof(struct Message));
    if (messages == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        closeMessageStore();
        return 1;
    }
    
    // Load messages
    int numLoaded = loadMessages(messages, maxMessages, sortByTime, ascending);
    if (numLoaded <= 0) {
        printf("No messages in history\n");
        free(messages);
        closeMessageStore();
        return 0;
    }
    
    // Display messages
    printf("Message History (%d messages):\n", numLoaded);
    printf("--------------------\n");
    
    for (int i = 0; i < numLoaded; i++) {
        char timeStr[64];
        struct tm* timeinfo = localtime(&messages[i].timestamp);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        printf("[%s] %s: %s\n", 
               timeStr, 
               messages[i].nickname, 
               messages[i].message);
    }
    
    printf("--------------------\n");
    
    // Clean up
    free(messages);
    closeMessageStore();
    return 0;
}
```

### 5. Update CMakeLists.txt
Update the build system to include the new files and executable:

```cmake
# Add message_store.c to the server target
add_executable(server server.c message_store.c chat.c)

# Add the history viewer executable
add_executable(history_viewer history_viewer.c message_store.c chat.c)
```

## Benefits of This Design

1. **Persistence**: Messages are stored and can be retrieved even after server restart
2. **Flexibility**: Messages can be sorted by time or nickname, in ascending or descending order
3. **Usability**: The server displays the last 10 messages on startup
4. **Extensibility**: The message store design allows for future enhancements

## Implementation Considerations

1. **File Format**: Binary format is used for simplicity and efficiency
2. **Memory Management**: Careful allocation and deallocation of resources
3. **Error Handling**: Robust error checking for file operations
4. **Compatibility**: Ensure the updated Message structure is compatible with existing code
5. **Testing**: Test with various sorting options and message volumes
