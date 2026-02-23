set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS" CACHE INTERNAL "")

# TOOD: use plog instead of loguru

CPMAddPackage(
  NAME loguru
  URL https://github.com/emilk/loguru/archive/refs/tags/v2.1.0.tar.gz
)

CPMAddPackage(
  NAME cbor-cpp
  GITHUB_REPOSITORY naphaso/cbor-cpp
  GIT_TAG 61dca7c
  DOWNLOAD_ONLY true
)

add_library(common STATIC
  Logger.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
  ${cbor-cpp_SOURCE_DIR}/src/encoder.cpp
  ${cbor-cpp_SOURCE_DIR}/src/output_dynamic.cpp
  Types.h
  Util.cpp
)

target_include_directories(common
PUBLIC
  ../
  ${loguru_SOURCE_DIR}
  ${cbor-cpp_SOURCE_DIR}/src
PRIVATE
  src
)

target_link_libraries(common
PUBLIC
  ${CMAKE_DL_LIBS}
  uvw_iot
)
