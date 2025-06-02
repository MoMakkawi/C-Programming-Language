#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include "chat.h"

/**
 * Initializes the message store by opening the specified file.
 * Must be called before saving or loading messages.
 *
 * @param filename The file used to persist messages
 * @return 0 on success, -1 on failure
 */
int initMessageStore(const char* filename);

/**
 * Appends a new message to the store.
 * The message is written in binary format.
 *
 * @param message The message to be stored
 * @return 0 on success, -1 on failure
 */
int saveMessage(const struct Message* message);

/**
 * Loads up to maxMessages from the store into the provided buffer.
 * Supports sorting by time or nickname and ordering (asc/desc).
 * Messages are read from a binary file written using saveMessage().
 *
 * @param messages Output array to fill with loaded messages
 * @param maxMessages Maximum number of messages to load
 * @param sortByTime If true, sort by timestamp; otherwise by nickname
 * @param ascending If true, sort in ascending order; false for descending
 * @return Number of messages successfully loaded, or -1 on failure
 */
int loadMessages(struct Message* messages, int maxMessages, int sortByTime, int ascending);

/**
 * Closes the underlying file used for message storage.
 * Should be called at the end of the program or after loading/saving is done.
 */
void closeMessageStore();

#endif
