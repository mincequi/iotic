find_package(Qt6 REQUIRED COMPONENTS WebSockets HttpServer)

add_library(websocket STATIC
  WebSocketExporter.cpp
)

target_link_libraries(websocket
PUBLIC
  Qt6::HttpServer
  Qt6::WebSockets
PRIVATE
  things
  webapp
)
