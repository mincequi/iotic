find_package(Catch2 REQUIRED)

add_executable(tests
  RulesTest.cpp
)

target_link_libraries(tests
PRIVATE
  Catch2::Catch2WithMain
  rules
)

include(CTest)
include(Catch)
catch_discover_tests(tests)
