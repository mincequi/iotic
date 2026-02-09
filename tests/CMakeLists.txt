CPMAddPackage(gh:catchorg/Catch2@3.12.0)

add_library(common_test STATIC
  common/TestThing.h common/TestThing.cpp
)
target_include_directories(common_test PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(common_test
PUBLIC
  Catch2::Catch2WithMain
  common
  things
)

add_executable(common_tests
  common/NLohmannTest.cpp

)
target_link_libraries(common_tests
PRIVATE
  common_test
)

add_executable(webserver_tests
  webserver/WebAppRouterTest.cpp
)
target_link_libraries(webserver_tests
PRIVATE
  common_test
  webserver
)

add_executable(strategies_tests
  strategies/MultiPhaseStrategyTest.cpp
)
target_link_libraries(strategies_tests
PRIVATE
  common_test
  strategies
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(common_tests)
catch_discover_tests(strategies_tests)
