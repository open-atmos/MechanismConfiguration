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

if(ENABLE_JSON)
  target_sources(mechanism_configuration
    PRIVATE
      json_parser.cpp
  )

  target_link_libraries(mechanism_configuration 
    PUBLIC 
      nlohmann_json::nlohmann_json
  )
endif()

if(ENABLE_YAML)
  target_sources(mechanism_configuration
    PRIVATE
      yaml_parser.cpp
  )

  target_link_libraries(mechanism_configuration 
    PUBLIC 
      yaml-cpp::yaml-cpp
  )
endif()
