find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebSockets)

add_library(websocket STATIC
  WebSocketExporter.cpp
)

target_link_libraries(websocket
PUBLIC
    Qt${QT_VERSION_MAJOR}::WebSockets
PRIVATE
  things
)
