#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE* messageFile = NULL;             // File pointer for message history
static char currentFilename[256] = {0};      // Stores the filename for later reuse (e.g., reading)

/**
 * Initializes the message store by opening the file for appending.
 */
int initMessageStore(const char* filename) {
    if (messageFile != NULL) {
        fclose(messageFile); // Close if already open
    }

    strncpy(currentFilename, filename, sizeof(currentFilename) - 1);

    // Open file in append mode to preserve history
    messageFile = fopen(filename, "a+");
    if (messageFile == NULL) {
        perror("Failed to open message store file");
        return -1;
    }

    return 0;
}

/**
 * Saves a message to the currently opened message file.
 */
int saveMessage(const struct Message* message) {
    if (messageFile == NULL || message == NULL) {
        return -1;
    }

    // Write binary message struct to file
    size_t written = fwrite(message, sizeof(struct Message), 1, messageFile);
    fflush(messageFile);  // Force flush to disk for durability

    return (written == 1) ? 0 : -1;
}

// Comparison function for qsort — sorts by timestamp
static int compareByTime(const void* a, const void* b) {
    const struct Message* msgA = (const struct Message*)a;
    const struct Message* msgB = (const struct Message*)b;

    if (msgA->timestamp < msgB->timestamp) return -1;
    if (msgA->timestamp > msgB->timestamp) return 1;
    return 0;
}

// Comparison function for qsort — sorts alphabetically by nickname
static int compareByNickname(const void* a, const void* b) {
    const struct Message* msgA = (const struct Message*)a;
    const struct Message* msgB = (const struct Message*)b;

    return strcmp(msgA->nickname, msgB->nickname);
}

/**
 * Loads messages from the file into memory, sorts them, and copies a limited
 * number into the provided buffer.
 */
int loadMessages(struct Message* messages, int maxMessages, int sortByTime, int ascending) {
    if (messages == NULL || maxMessages <= 0) {
        return -1;
    }

    // Reopen file if it wasn't already opened by initMessageStore
    if (messageFile == NULL) {
        messageFile = fopen(currentFilename, "r");
        if (messageFile == NULL) {
            perror("Failed to open message store file for reading");
            return -1;
        }
    }

    rewind(messageFile);  // Reset file cursor to beginning

    // Read messages into dynamically sized array
    struct Message* allMessages = NULL;
    int numMessages = 0;
    int capacity = 100;  // Start with space for 100 messages

    allMessages = malloc(capacity * sizeof(struct Message));
    if (allMessages == NULL) {
        return -1;
    }

    while (fread(&allMessages[numMessages], sizeof(struct Message), 1, messageFile) == 1) {
        numMessages++;

        // Expand array if needed
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

    // Sort messages using selected criteria
    if (sortByTime) {
        qsort(allMessages, numMessages, sizeof(struct Message), compareByTime);
    } else {
        qsort(allMessages, numMessages, sizeof(struct Message), compareByNickname);
    }

    // Reverse the array in-place if descending order requested
    if (!ascending) {
        for (int i = 0; i < numMessages / 2; i++) {
            struct Message temp = allMessages[i];
            allMessages[i] = allMessages[numMessages - 1 - i];
            allMessages[numMessages - 1 - i] = temp;
        }
    }

    // Copy up to maxMessages into the output buffer
    int messagesToCopy = (numMessages < maxMessages) ? numMessages : maxMessages;
    memcpy(messages, allMessages, messagesToCopy * sizeof(struct Message));

    free(allMessages);
    return messagesToCopy;
}

/**
 * Closes the message file if open.
 */
void closeMessageStore() {
    if (messageFile != NULL) {
        fclose(messageFile);
        messageFile = NULL;
    }
}
