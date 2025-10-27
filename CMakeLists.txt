cmake_minimum_required(VERSION 3.14)

project(iotic VERSION 0.4.0 LANGUAGES C CXX)

include(cmake/CPM.cmake)

CPMAddPackage("gh:mincequi/uvw_iot#0fed7c5")

set(CMAKE_CXX_STANDARD 11)

option(BUILD_TESTING "Build test programs" OFF)

find_package(Threads)

add_subdirectory(src/cli)
add_subdirectory(src/common)
add_subdirectory(src/config)
#add_subdirectory(src/fronius)
#add_subdirectory(src/influx)
add_subdirectory(src/strategies)
add_subdirectory(src/rules)
add_subdirectory(src/things)
add_subdirectory(src/webapp/build)
add_subdirectory(src/webserver)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_executable(iotic
  src/main.cpp
  src/appDaemon/AppBackend.cpp
  src/appDiscover/AppDiscover.cpp
)

target_link_libraries(iotic
  Threads::Threads
  uvw_iot
  uvw_net::dns_sd
  ${CMAKE_DL_LIBS}
  cli
  common
  config
  #influx
  #modbus
  #mqtt
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
