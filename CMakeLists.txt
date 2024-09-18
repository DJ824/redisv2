cmake_minimum_required(VERSION 3.27)
project(redisv2)

set(CMAKE_CXX_STANDARD 17)


set(BOOST_ROOT "/opt/homebrew/opt/boost")
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@1.1")

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(server
        server/server.cpp
        structures/data_store.cpp
        structures/skip_list.cpp
)

add_executable(client
        client/client.cpp
)

add_executable(data_structure_tests
        tests/data_structure_tests.cpp
        structures/data_store.cpp
        structures/skip_list.cpp
)

add_custom_target(redisv2 ALL DEPENDS server client data_structure_tests)

target_link_libraries(server PRIVATE
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_link_libraries(client PRIVATE
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_link_libraries(data_structure_tests PRIVATE
        gtest_main
)

target_include_directories(server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)

target_include_directories(client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)

target_include_directories(data_structure_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
)

include(GoogleTest)
gtest_discover_tests(data_structure_tests)