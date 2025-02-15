################################################################################
# Preamble
cmake_minimum_required(VERSION 3.21)

project(
  mechanism_configuration
  VERSION 1.0.0
  LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

message(STATUS "CMake build configuration for ${PROJECT_NAME} (${CMAKE_BUILD_TYPE}) ${PROJECT_VERSION}")

################################################################################
# Projet wide setup options

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

option(OPEN_ATMOS_ENABLE_TESTS "Build the tests" ON)

################################################################################
# Dependencies

include(cmake/dependencies.cmake)

################################################################################
# project source code

add_subdirectory(src)

################################################################################
# Tests

if(PROJECT_IS_TOP_LEVEL AND OPEN_ATMOS_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)

  # Copy example folder to use as test data

  add_custom_target(copy_example_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_BINARY_DIR}/examples)
endif()

################################################################################
# Packaging

# only include packaging if we are the top level project being built
if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(packaging)
endif()
