include(FetchContent)
FetchContent_Declare(tomlcpp
    GIT_REPOSITORY    https://github.com/cktan/tomlcpp.git
    GIT_TAG           4212f1fccf530e276a2e1b63d3f99fbfb84e86a4
)
FetchContent_MakeAvailable(tomlcpp)

include(FetchContent)
FetchContent_Declare(tomlplusplus
    URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.3.0.tar.gz
    #GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    #GIT_TAG        v3.3.0
)
if(NOT tomlplusplus_POPULATED)
    FetchContent_Populate(tomlplusplus)
    add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(tomlcpp STATIC
  ${tomlcpp_SOURCE_DIR}/toml.c
  ${tomlcpp_SOURCE_DIR}/tomlcpp.cpp
)

add_library(config STATIC
  #Config_tomlcpp.cpp
  Config_tomlplusplus.cpp
)

target_include_directories(config
PRIVATE
  #${tomlcpp_SOURCE_DIR}
  ${tomlplusplus_SOURCE_DIR}
)

target_link_libraries(config
PRIVATE
  common
  #tomlcpp
)
