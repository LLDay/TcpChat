find_package(Threads)

add_library(chatlibs SHARED ${CMAKE_SOURCE_DIR}/src/server.cpp
                            ${CMAKE_SOURCE_DIR}/src/connection.cpp)

target_link_libraries(chatlibs ${CMAKE_THREAD_LIBS_INIT})

add_executable(server ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(server chatlibs)
