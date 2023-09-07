Set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

## We cannot use this official release, because it wants the examples and tests
## to be built. So, we are using latest master.
#FetchContent_Declare(rxcpp
#  URL https://github.com/ReactiveX/RxCpp/archive/refs/tags/v4.1.1.tar.gz
#)
#FetchContent_MakeAvailable(rxcpp)

#set(RXCPP_DISABLE_TESTS_AND_EXAMPLES ON)
#FetchContent_Declare(rxcpp
#  GIT_REPOSITORY https://github.com/ReactiveX/RxCpp.git
#  GIT_TAG        761b932a80e2be6e2b62d232e754bd96fc448946 # main from Mar 13, 2022
#  GIT_PROGRESS TRUE
#)
#FetchContent_MakeAvailable(rxcpp)

add_subdirectory(goe)
add_subdirectory(http)
add_subdirectory(shelly)
add_subdirectory(sunspec)

add_library(things STATIC
  Site.cpp
  Thing.cpp
  ThingFactory.cpp
  ThingInfo.cpp
  ThingValue.cpp
  ThingsDiscovery.cpp
  ThingsManager.cpp
  ThingsRepository.cpp
)

target_link_libraries(things
PUBLIC
  common
  config
  goe
  shelly
  sunspec
)

target_include_directories(things
PUBLIC
  ../
)
