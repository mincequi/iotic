cmake_minimum_required(VERSION 3.14)

project(elsewhere
  VERSION 0.1.0
  LANGUAGES CXX)

Include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

#FetchContent_Declare(
#  Catch2
#  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#  GIT_TAG        v3.0.0-preview3
#)
#FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v2.5.0
)
# This is needed to exclude .h and .a files from installation
if(NOT tomlplusplus_POPULATED)
    FetchContent_Populate(tomlplusplus)
    add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

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
  src/Config.cpp
  src/Statistics.cpp
)

target_link_libraries(elsewhere
  Threads::Threads
  ${CMAKE_DL_LIBS}
  tomlplusplus::tomlplusplus
  common
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
