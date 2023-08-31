find_package(Qt6 REQUIRED COMPONENTS WebSockets HttpServer)

add_library(webserver STATIC
  WebServer.cpp
)

target_link_libraries(webserver
PUBLIC
  Qt6::HttpServer
  Qt6::WebSockets
PRIVATE
  things
  webapp
)
