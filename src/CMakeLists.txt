################################################################################
# version
configure_file(version.hpp.in ${PROJECT_SOURCE_DIR}/include/mechanism_configuration/version.hpp @ONLY)

add_library(mechanism_configuration)
add_library(open_atmos::mechanism_configuration ALIAS mechanism_configuration)

target_compile_features(mechanism_configuration PUBLIC cxx_std_20)

target_sources(mechanism_configuration
  PRIVATE
    parse_status.cpp
    validate_schema.cpp
)

target_include_directories(mechanism_configuration
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(v0)
add_subdirectory(v1)

target_link_libraries(mechanism_configuration 
  PUBLIC 
    yaml-cpp::yaml-cpp
)
