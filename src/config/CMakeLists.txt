include(FetchContent)

FetchContent_Declare(toml11
    URL https://github.com/ToruNiina/toml11/archive/refs/tags/v3.7.1.tar.gz
)
if(NOT toml11_POPULATED)
    FetchContent_Populate(toml11)
endif()

add_library(config
STATIC
  Config.cpp
)

target_include_directories(config
PRIVATE
  ${toml11_SOURCE_DIR}
)

target_link_libraries(config
PRIVATE
  common
)
