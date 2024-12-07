cmake_minimum_required(VERSION 3.11)
project(home-control)

set(CMAKE_CXX_STANDARD 17)
if (UNIX)
    add_compile_options(-Wall -Wextra)
endif()

option(BUILD_TESTS "Build unit tests" ON)

# find our custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(mosquitto REQUIRED)
find_package(libconfig REQUIRED)
find_package(libgpiod REQUIRED)
find_package(nlohmann_json REQUIRED)

if(BUILD_TESTS)
    set(GOOGLETEST_DIR "/usr/src/googletest" CACHE STRING "Location of googletest sources")
    add_subdirectory(${GOOGLETEST_DIR} gtest)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libs/mqttbase)
add_subdirectory(apps/autolight)
add_subdirectory(apps/gpio_ctrl)
add_subdirectory(apps/value_action)
