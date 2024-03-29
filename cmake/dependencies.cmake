include(FetchContent)

################################################################################
# google test

if(PROJECT_IS_TOP_LEVEL AND ENABLE_TESTS)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )

  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)
endif()

################################################################################
# nlohmann::json

if(ENABLE_JSON)
  FetchContent_Declare(json
      GIT_REPOSITORY https://github.com/nlohmann/json.git
      GIT_TAG v3.11.2
  )
  FetchContent_MakeAvailable(json)
endif()

################################################################################
# yaml-cpp

if(ENABLE_YAML)
  FetchContent_Declare(yaml
      GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
      GIT_TAG 0.8.0
  )
  FetchContent_MakeAvailable(yaml)
endif()