cmake_minimum_required(VERSION 3.0)

find_package(GTest)

if(GTEST_FOUND)
    add_executable(chat_test
        ${CMAKE_SOURCE_DIR}/test/test_main.cpp
        ${CMAKE_SOURCE_DIR}/test/chat_test.cpp)

    target_link_libraries(chat_test PRIVATE
        chatlibs ${GTEST_BOTH_LIBRARIES})

    add_test(
        NAME chat_test
        COMMAND chat_test)
endif()
