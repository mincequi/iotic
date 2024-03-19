find_package(Catch2 REQUIRED)

add_executable(common_tests
  common/NLohmannTest.cpp
)
target_link_libraries(common_tests
PRIVATE
  Catch2::Catch2WithMain
  common
  things
)

add_executable(webserver_tests
  webserver/WebAppRouterTest.cpp
)
target_link_libraries(webserver_tests
PRIVATE
  Catch2::Catch2WithMain
  common
  webserver
)

include(CTest)
include(Catch)
#catch_discover_tests(tests)
