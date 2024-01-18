################################################################################
# Test utilities

include(test_util)

################################################################################
# Tests

create_standard_test(NAME parse_species SOURCES test_parse_species.cpp)
create_standard_test(NAME parse_phases SOURCES test_parse_phases.cpp)
create_standard_test(NAME parse_arrhenius SOURCES test_parse_arrhenius.cpp)
create_standard_test(NAME parse_troe SOURCES test_parse_troe.cpp)
create_standard_test(NAME parse_branched SOURCES test_parse_branched.cpp)
create_standard_test(NAME parse_tunneling SOURCES test_parse_tunneling.cpp)
create_standard_test(NAME parse_surface SOURCES test_parse_surface.cpp)

################################################################################
# Copy test data

add_custom_target(copy_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_configs ${CMAKE_BINARY_DIR}/unit_configs)