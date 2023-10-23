Set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
