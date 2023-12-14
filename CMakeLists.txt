cmake_minimum_required(VERSION 3.14)

project(iotic VERSION 0.3.2 LANGUAGES C CXX)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

option(BUILD_TESTS "Build test programs" OFF)

find_package(Threads)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network SerialBus REQUIRED)

add_subdirectory(src/cli)
add_subdirectory(src/common)
add_subdirectory(src/config)
add_subdirectory(src/http)
#add_subdirectory(src/influx)
add_subdirectory(src/modbus)
add_subdirectory(src/mqtt)
add_subdirectory(src/strategies)
add_subdirectory(src/rules)
add_subdirectory(src/things)
add_subdirectory(src/webapp)
add_subdirectory(src/webserver)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

add_executable(iotic
  src/main.cpp
  src/Statistics.cpp
  src/appDaemon/AppBackend.cpp
  src/appDiscover/AppDiscover.cpp
)

target_link_libraries(iotic
  Threads::Threads
  ${CMAKE_DL_LIBS}
  cli
  common
  config
  #influx
  modbus
  mqtt
  rules
  things
  webserver
)

install(TARGETS iotic DESTINATION bin)
#install(FILES ${PROJECT_SOURCE_DIR}/etc/iotic.conf DESTINATION /var/lib/iotic)
install(FILES ${PROJECT_SOURCE_DIR}/debian/iotic.service DESTINATION /usr/lib/systemd/system)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Weichselbaumer <mincequi@web.de>") #required
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS adduser)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/debian/postinst;${PROJECT_SOURCE_DIR}/debian/postrm")
include(CPack)
