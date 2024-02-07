################################################################################
# Test utilities

include(test_util)

################################################################################
# Tests
if(ENABLE_JSON)
  create_standard_test(NAME json_parser SOURCES test_json_parser.cpp)
endif()

if(ENABLE_YAML)
  create_standard_test(NAME yaml_parser SOURCES test_yaml_parser.cpp)
endif()