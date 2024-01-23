################################################################################
# Test utilities

include(test_util)

################################################################################
# Tests

create_standard_test(NAME parse_arrhenius SOURCES test_parse_arrhenius.cpp)
create_standard_test(NAME parse_branched SOURCES test_parse_branched.cpp)
create_standard_test(NAME parse_condensed_phase_arrhenius SOURCES test_parse_condensed_phase_arrhenius.cpp)
create_standard_test(NAME parse_condensed_phase_photolysis SOURCES test_parse_condensed_phase_photolysis.cpp)
create_standard_test(NAME parse_emission SOURCES test_parse_emission.cpp)
create_standard_test(NAME parse_first_order_loss SOURCES test_parse_first_order_loss.cpp)
create_standard_test(NAME parse_henrys_law SOURCES test_parse_henrys_law.cpp)
create_standard_test(NAME parse_phases SOURCES test_parse_phases.cpp)
create_standard_test(NAME parse_photolysis SOURCES test_parse_photolysis.cpp)
create_standard_test(NAME parse_species SOURCES test_parse_species.cpp)
create_standard_test(NAME parse_surface SOURCES test_parse_surface.cpp)
create_standard_test(NAME parse_troe SOURCES test_parse_troe.cpp)
create_standard_test(NAME parse_tunneling SOURCES test_parse_tunneling.cpp)
create_standard_test(NAME parse_aqueous_equilibrium SOURCES test_parse_aqueous_equilibrium.cpp)
create_standard_test(NAME parse_wet_deposition SOURCES test_parse_wet_deposition.cpp)

################################################################################
# Copy test data

add_custom_target(copy_unit_test_configs ALL ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_configs ${CMAKE_BINARY_DIR}/unit_configs)