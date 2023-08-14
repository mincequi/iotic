cmake_minimum_required(VERSION 3.14)

project(elsewhere
  VERSION 0.1.0
  LANGUAGES C CXX)

Include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Threads)
find_package(QT NAMES Qt5 Qt6 COMPONENTS Core Network SerialBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network SerialBus REQUIRED)

add_subdirectory(src/common)
add_subdirectory(src/config)
#add_subdirectory(src/feed)
add_subdirectory(src/influx)
#add_subdirectory(src/iot)
add_subdirectory(src/modbus)
add_subdirectory(src/mqtt)
add_subdirectory(src/things)
#add_subdirectory(src/ui)

add_executable(elsewhere
  src/main.cpp
  src/AppBackend.cpp
  src/Statistics.cpp
)

target_link_libraries(elsewhere
  Threads::Threads
  ${CMAKE_DL_LIBS}
  common
  config
  #feed
  influx
  #iot
  modbus
  mqtt
  things
  #ui
)

install(TARGETS elsewhere DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/etc/elsewhere.conf DESTINATION /etc)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Weichselbaumer <mincequi@web.de>") #required
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS adduser)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/debian/postinst;${PROJECT_SOURCE_DIR}/debian/postrm")
include(CPack)
