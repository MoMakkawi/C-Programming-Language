#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chat.h"
#include "message_store.h"

#define DEFAULT_HISTORY_FILE "chat_history.dat"

void printUsage(const char* programName) {
    printf("Usage: %s [options]\n", programName);
    printf("Options:\n");
    printf("  -f <filename>   Specify message history file (default: %s)\n", DEFAULT_HISTORY_FILE);
    printf("  -n <number>     Number of messages to display (default: all)\n");
    printf("  -t              Sort by timestamp (default)\n");
    printf("  -u              Sort by username/nickname\n");
    printf("  -a              Sort in ascending order (default)\n");
    printf("  -d              Sort in descending order\n");
    printf("  -h              Display this help message\n");
}

int main(int argc, char** argv) {
    char filename[256] = DEFAULT_HISTORY_FILE;
    int maxMessages = -1;  // -1 means display all available messages
    int sortByTime = 1;    // Default: sort messages by timestamp
    int ascending = 1;     // Default: sort in ascending order

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strncpy(filename, argv[++i], sizeof(filename) - 1); // Override default file
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            maxMessages = atoi(argv[++i]); // Limit number of messages to show
            if (maxMessages <= 0) {
                fprintf(stderr, "Invalid number of messages\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            sortByTime = 1;
        } else if (strcmp(argv[i], "-u") == 0) {
            sortByTime = 0; // Sort by nickname instead of timestamp
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

    // Open the message store file for reading
    if (initMessageStore(filename) != 0) {
        fprintf(stderr, "Failed to open message history file: %s\n", filename);
        return 1;
    }

    // If user didn't specify how many messages, determine from file size
    if (maxMessages <= 0) {
        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
            fprintf(stderr, "Failed to open message history file: %s\n", filename);
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file); // Get total file size in bytes
        maxMessages = fileSize / sizeof(struct Message); // Calculate number of stored messages
        fclose(file);

        if (maxMessages <= 0) {
            printf("No messages in history\n");
            return 0;
        }
    }

    // Allocate memory to hold the messages
    struct Message* messages = malloc(maxMessages * sizeof(struct Message));
    if (messages == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        closeMessageStore();
        return 1;
    }

    // Load messages from the file with specified sorting options
    int numLoaded = loadMessages(messages, maxMessages, sortByTime, ascending);
    if (numLoaded <= 0) {
        printf("No messages in history\n");
        free(messages);
        closeMessageStore();
        return 0;
    }

    // Display loaded messages with timestamp formatting
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
