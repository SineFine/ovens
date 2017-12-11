cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(server)

set (server_VERSION_MAJOR 1)
set (server_VERSION_MINOR 0)

message(STATUS "System: " ${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})

include(CheckIncludeFileCXX)
set(CMAKE_REQUIRED_INCLUDES "/usr/local/include")
CHECK_INCLUDE_FILE_CXX(iostream  STD_iostream)
CHECK_INCLUDE_FILE_CXX(chrono  STD_chrono)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0 -g -Wall -Wextra -O0")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
  find_package(Boost COMPONENTS system filesystem signals thread program_options date_time REQUIRED)
  include_directories( ${Boost_INCLUDE_DIRS} )
endif(UNIX)

if(Boost_FOUND)
  find_library(BOOST_SYSTEM boost_system)
  find_library(BOOST_THREAD boost_thread)
  find_library(SIGNALS_LIB boost_signals)
endif(Boost_FOUND)

find_library(PTHREAD_LIB pthread)
find_library(MYSQLC_LIB mysqlclient)
find_library(MYSQLCPP_LIB mysqlcppconn)

add_custom_target(headers SOURCES logos.h macro.h)

add_library(service STATIC service.cpp service.h)
add_library(acceptor STATIC acceptor.cpp acceptor.h)
add_library(client STATIC client.cpp client.h)
add_library(skeleton STATIC skeleton.cpp skeleton.h)
add_library(mydb STATIC mydb.cpp mydb.h)
add_library(time mytime.h mytime.cpp)

include_directories()

set(SOURCE_FILE "main.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILE})

target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIB} ${BOOST_SYSTEM} ${BOOST_THREAD}
                      ${MYSQLC_LIB} ${MYSQLCPP_LIB}
                      acceptor client service skeleton mydb time)
