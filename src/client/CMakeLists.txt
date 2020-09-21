cmake_minimum_required(VERSION 3.5)

find_package(Qt5 COMPONENTS Widgets Network Core REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt_wrap_ui(client_ui
    ${CMAKE_SOURCE_DIR}/ui/client.ui
    ${CMAKE_SOURCE_DIR}/ui/dialog_name.ui
    ${CMAKE_SOURCE_DIR}/ui/message_widget.ui)

qt_wrap_cpp(client_moc
    ${CMAKE_SOURCE_DIR}/include/client/client.h
    ${CMAKE_SOURCE_DIR}/include/client/dialog_name.h
    ${CMAKE_SOURCE_DIR}/include/client/event_listener_adapter.h
    ${CMAKE_SOURCE_DIR}/include/client/message_widget.h)

add_executable(client ${client_moc} ${client_ui}
    ${CMAKE_SOURCE_DIR}/src/client/client_main.cpp
    ${CMAKE_SOURCE_DIR}/src/client/client.cpp
    ${CMAKE_SOURCE_DIR}/src/client/dialog_name.cpp
    ${CMAKE_SOURCE_DIR}/src/client/event_listener_adapter.cpp
    ${CMAKE_SOURCE_DIR}/src/client/message_widget.cpp)

target_link_libraries(client PRIVATE
    chatlibs Qt5::Widgets Qt5::Network Qt5::Core)
