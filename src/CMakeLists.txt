find_package(Threads)

if(PRINT_ERRORS)
    add_definitions(-DCHAT_LOG_ERROR=1)
endif()

if(PRINT_INFO)
    add_definitions(-DCHAT_LOG_INFO=1)
endif()

add_library(
    chatlibs STATIC
    ${CMAKE_SOURCE_DIR}/src/incoming_events_listener.cpp
    ${CMAKE_SOURCE_DIR}/src/io_operations.cpp
    ${CMAKE_SOURCE_DIR}/src/looped_thread.cpp
    ${CMAKE_SOURCE_DIR}/src/manual_control.cpp
    ${CMAKE_SOURCE_DIR}/src/message.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/worker.cpp
    ${CMAKE_SOURCE_DIR}/src/workers_pool.cpp)

target_link_libraries(chatlibs PUBLIC ${CMAKE_THREAD_LIBS_INIT})

add_executable(server
    ${CMAKE_SOURCE_DIR}/src/connection_listener.cpp
    ${CMAKE_SOURCE_DIR}/src/server.cpp
    ${CMAKE_SOURCE_DIR}/src/server_main.cpp)

target_link_libraries(server PRIVATE chatlibs)
