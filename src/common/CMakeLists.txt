set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS" CACHE INTERNAL "")

FetchContent_Declare(loguru
  GIT_REPOSITORY https://github.com/emilk/loguru.git
  GIT_TAG        v2.1.0
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(loguru)

CPMAddPackage("gh:gabime/spdlog@1.13.0")

add_library(common STATIC
  Logger.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
  Types.h
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
