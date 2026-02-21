set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS" CACHE INTERNAL "")

# TOOD: use plog instead of loguru

CPMAddPackage(
  NAME loguru
  URL https://github.com/emilk/loguru/archive/refs/tags/v2.1.0.tar.gz
)

add_library(common STATIC
  Logger.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
  Types.h
  Util.cpp
)

target_include_directories(common
PUBLIC
  ../
  ${loguru_SOURCE_DIR}
PRIVATE
  src
)

target_link_libraries(common
PUBLIC
  ${CMAKE_DL_LIBS}
  uvw_iot
)
