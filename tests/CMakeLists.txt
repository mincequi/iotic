find_package(Catch2 REQUIRED)

add_executable(tests
  #RppTest.cpp
  #CppTest.cpp
  RulesTest.cpp
  TestUtil.cpp
)

target_link_libraries(tests
PRIVATE
  Catch2::Catch2WithMain
  #Catch2::Catch2
  common
  rules
  things
)

include(CTest)
include(Catch)
catch_discover_tests(tests)
