################################################################################
# Test utilities

include(test_util)

################################################################################
# Tests

create_standard_test(NAME parser SOURCES test_parser.cpp)

################################################################################
# Copy test data

add_custom_target(copy_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_configs ${CMAKE_BINARY_DIR}/unit_configs)