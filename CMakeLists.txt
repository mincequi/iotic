cmake_minimum_required(VERSION 3.14)

project(elsewhere LANGUAGES CXX)

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

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(qmqtt_SHARED OFF)
add_definitions(-DMQTT_PROJECT_INCLUDE_SRC)
FetchContent_Declare(qmqtt
  GIT_REPOSITORY https://github.com/emqx/qmqtt.git
  GIT_TAG        v1.0.0
  GIT_PROGRESS TRUE
)
# This is needed to exclude .h and .a files from installation
if(NOT qmqtt_POPULATED)
    FetchContent_Populate(qmqtt)
    add_subdirectory(${qmqtt_SOURCE_DIR} ${qmqtt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/common)
add_subdirectory(src/feed)
add_subdirectory(src/iot)
#add_subdirectory(src/ui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Threads)
#find_package(QT NAMES Qt5 COMPONENTS Core Network SerialBus REQUIRED)
find_package(Qt5 COMPONENTS Core Network SerialBus REQUIRED)

add_executable(elsewhere
  src/main.cpp
  src/AppBackend.cpp
  #src/AppFrontend.cpp
  src/Config.cpp
  #src/DispatchObject.cpp
  #src/DispatchThread.cpp
  src/MqttExporter.cpp
  src/Statistics.cpp
  #src/WQApplication.cpp
)

target_link_libraries(elsewhere
  Threads::Threads
  ${CMAKE_DL_LIBS}
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  qmqtt
  tomlplusplus::tomlplusplus
  common
  feed
  iot
  #ui
)

find_package(curl)
if(CURL_FOUND)
    set(BUILD_SHARED_LIBS OFF)
    set(INFLUXCXX_WITH_BOOST OFF)
    set(INFLUXCXX_TESTING OFF)
    set(INFLUXCXX_SYSTEMTEST OFF)
    FetchContent_Declare(
      influxdb
      GIT_REPOSITORY https://github.com/offa/influxdb-cxx.git
      GIT_TAG        v0.6.7
    )
    # This is needed to exclude .h and .a files from installation
    if(NOT influxdb_POPULATED)
	FetchContent_Populate(influxdb)
	add_subdirectory(${influxdb_SOURCE_DIR} ${influxdb_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_sources(elsewhere
	PRIVATE
	src/InfluxExporter.cpp
    )
    target_link_libraries(elsewhere
	InfluxData::InfluxDB
    )
    add_compile_definitions(
	USE_INFLUXDB
    )
endif(CURL_FOUND)

install(TARGETS elsewhere DESTINATION bin)
install(FILES etc/elsewhere.conf TYPE SYSCONF)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Weichselbaumer <mincequi@web.de>") #required
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)
#set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS adduser)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${PROJECT_SOURCE_DIR}/debian/postinst;
    ${PROJECT_SOURCE_DIR}/debian/postrm")
include(CPack)
