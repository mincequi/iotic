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

add_library(config
STATIC
  Config.cpp
)

target_include_directories(config
PRIVATE
  ${tomlplusplus_SOURCE_DIR}
)

target_link_libraries(config
PRIVATE
  common
)
