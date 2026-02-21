CPMAddPackage("gh:erthink/libmdbx@0.13.11")
CPMAddPackage(
  NAME clipp
  # clipp version of GerHobbelt produces a lot of warnings and seems not so nice
  #URL https://github.com/GerHobbelt/clipp/archive/refs/tags/v2023.10.12.tar.gz
  URL https://github.com/muellan/clipp/archive/refs/tags/v1.2.3.tar.gz
)

add_library(cli STATIC
  Cli.cpp
)

target_include_directories(cli
PRIVATE
  ${clipp_SOURCE_DIR}/include
)
