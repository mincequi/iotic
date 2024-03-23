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

FetchContent_Declare(llhttp
  URL https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.0.tar.gz
  GIT_PROGRESS    TRUE
)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
FetchContent_MakeAvailable(llhttp)

add_library(httpthing STATIC
  HttpClient.cpp
  HttpDiscovery.cpp
  HttpRequest.cpp
  HttpResponse.cpp
  HttpSession.cpp
  HttpThing.cpp
  HttpThingFactory.cpp
)

target_include_directories(httpthing
PRIVATE
  ${lhttp_SOURCE_DIR}/include
)

target_link_libraries(httpthing
PUBLIC
  common
  goe
  qmdnsengine
  shelly
  llhttp_static
)
