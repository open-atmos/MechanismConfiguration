################################################################################
# version
configure_file(version.hpp.in ${PROJECT_SOURCE_DIR}/include/open_atmos/mechanism_configuration/version.hpp @ONLY)

add_library(mechanism_configuration)
add_library(open_atmos::mechanism_configuration ALIAS mechanism_configuration)

target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
)

message(STATUS "Path: ${PROJECT_SOURCE_DIR}/include")

target_include_directories(mechanism_configuration
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mechanism_configuration PUBLIC nlohmann_json::nlohmann_json)