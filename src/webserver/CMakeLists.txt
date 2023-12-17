FetchContent_Declare(uws
  URL           https://github.com/uNetworking/uWebSockets/archive/refs/tags/v20.47.0.tar.gz
  GIT_PROGRESS  TRUE
)
FetchContent_MakeAvailable(uws)

FetchContent_Declare(us
  URL           https://github.com/uNetworking/uSockets/archive/refs/tags/v0.8.6.tar.gz
  GIT_PROGRESS  TRUE
)
FetchContent_MakeAvailable(us)

add_library(webserver STATIC
  OcppBehavior.cpp
  WebAppBehavior.cpp
  WebAppRouter.cpp
  WebServer.cpp
  ${us_SOURCE_DIR}/src/bsd.c
  ${us_SOURCE_DIR}/src/context.c
  ${us_SOURCE_DIR}/src/loop.c
  ${us_SOURCE_DIR}/src/socket.c
  ${us_SOURCE_DIR}/src/eventing/libuv.c
)

target_compile_definitions(webserver
PRIVATE
  -DLIBUS_NO_SSL
  -DLIBUS_USE_LIBUV
  -DUWS_NO_ZLIB
  -DCMRC_NO_EXCEPTIONS
)

target_include_directories(webserver
PRIVATE
  ${us_SOURCE_DIR}/src
  ${uws_SOURCE_DIR}/src
)

target_link_libraries(webserver
PRIVATE
  things
  webapp
)
