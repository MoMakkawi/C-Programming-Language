cmake_minimum_required(VERSION 3.16...3.23)
project(ChatMScIT LANGUAGES C)
find_package(Doxygen)

# Add message_store.c to the server target
add_executable(server server.c chat.c message_store.c)

# Add the client executable
add_executable(client client.c chat.c)

# Add the history viewer executable
add_executable(history_viewer history_viewer.c chat.c message_store.c)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found")
    doxygen_add_docs(chatdoc ${PROJECT_SOURCE_DIR})
endif()
