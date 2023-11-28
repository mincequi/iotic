FetchContent_Declare(clipp
    URL https://github.com/muellan/clipp/archive/refs/tags/v1.2.3.tar.gz
)
if(NOT clipp_POPULATED)
    FetchContent_Populate(clipp)
endif()

add_library(cli STATIC
  Cli.cpp
)

target_include_directories(cli
PRIVATE
  ${clipp_SOURCE_DIR}/include
)
