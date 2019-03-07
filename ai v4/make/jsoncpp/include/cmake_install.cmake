# Install script for directory: /home/ali/Desktop/AI/ai v4/jsoncpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/json" TYPE FILE FILES
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/allocator.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/assertions.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/autolink.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/config.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/features.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/forwards.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/json.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/reader.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/value.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/version.h"
    "/home/ali/Desktop/AI/ai v4/jsoncpp/include/json/writer.h"
    )
endif()

