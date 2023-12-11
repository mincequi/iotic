set(FETCHCONTENT_QUIET FALSE)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(BUILD_TESTS OFF)

FetchContent_Declare(qmdns
  GIT_REPOSITORY  https://github.com/nitroshare/qmdnsengine.git
  GIT_TAG         0ca80117e853671d909b3cec9e2bdcac85a13b9f # main from Jan 16, 2022
  GIT_PROGRESS    TRUE
)
if(NOT qmdns_POPULATED)
  FetchContent_Populate(qmdns)
  set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries")
  add_subdirectory(${qmdns_SOURCE_DIR} ${qmdns_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(lhttp
  GIT_REPOSITORY  https://github.com/neithern/libhttp.git
  GIT_TAG         2a19dabe1337c03e9499b95dafc81105e63dc197
  GIT_PROGRESS    TRUE
)
if(NOT lhttp_POPULATED)
  FetchContent_Populate(lhttp)
endif()
#FetchContent_MakeAvailable(lhttp)

add_library(httpthing STATIC
  HttpDiscovery.cpp
  HttpThing.cpp
  HttpThingFactory.cpp
  ${lhttp_SOURCE_DIR}/src/buffer-pool.cpp
  ${lhttp_SOURCE_DIR}/src/chunked-decoder.cpp
  ${lhttp_SOURCE_DIR}/src/client.cpp
  ${lhttp_SOURCE_DIR}/src/content-writer.cpp
  ${lhttp_SOURCE_DIR}/src/loop.cpp
  ${lhttp_SOURCE_DIR}/src/parser.cpp
  ${lhttp_SOURCE_DIR}/src/uri.cpp
  ${lhttp_SOURCE_DIR}/src/utils.cpp
  ${lhttp_SOURCE_DIR}/src/pico/picohttpparser.c
)

target_include_directories(httpthing
PRIVATE
  ${lhttp_SOURCE_DIR}/include
)

target_link_libraries(httpthing
PUBLIC
  common
  qmdnsengine
)
