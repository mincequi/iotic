set(FETCHCONTENT_QUIET FALSE)

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
  HttpResponseParser.cpp
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
  shelly
  llhttp_static
  uvw_net
)
