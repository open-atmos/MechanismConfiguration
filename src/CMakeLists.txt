################################################################################
# version
configure_file(version.hpp.in ${PROJECT_SOURCE_DIR}/include/open_atmos/mechanism_configuration/version.hpp @ONLY)

add_library(mechanism_configuration)
add_library(open_atmos::mechanism_configuration ALIAS mechanism_configuration)

target_compile_features(mechanism_configuration PUBLIC cxx_std_17)

target_sources(mechanism_configuration
  PRIVATE
    parse_status.cpp
)

target_include_directories(mechanism_configuration
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    utils.cpp
    validation.cpp
    condensed_phase_arrhenius_parser.cpp
    troe_parser.cpp
    branched_parser.cpp
    tunneling_parser.cpp
    surface_parser.cpp
)

target_link_libraries(mechanism_configuration 
  PUBLIC 
    yaml-cpp::yaml-cpp
)