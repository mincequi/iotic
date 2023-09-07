include(FetchContent)
FetchContent_Declare(tomlcpp
    GIT_REPOSITORY    https://github.com/cktan/tomlcpp.git
    GIT_TAG           4212f1fccf530e276a2e1b63d3f99fbfb84e86a4
)
#option(BUILD_SHARED_LIBS "" OFF)
# This is needed to exclude .h and .a files from installation
#if(NOT tomlcpp_POPULATED)
#    FetchContent_Populate(tomlcpp)
#    add_subdirectory(${tomlcpp_SOURCE_DIR} ${tomlcpp_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()
FetchContent_MakeAvailable(tomlcpp)
add_library(tomlcpp STATIC
  ${tomlcpp_SOURCE_DIR}/toml.c
  ${tomlcpp_SOURCE_DIR}/tomlcpp.cpp
)

add_library(config STATIC
  Config.cpp
)

target_include_directories(config
PRIVATE
  ${tomlcpp_SOURCE_DIR}
)

target_link_libraries(config
PRIVATE
  common
  tomlcpp
)
