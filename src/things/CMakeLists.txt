set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(goe)
add_subdirectory(http)
add_subdirectory(shelly)
add_subdirectory(sunspec)

add_library(things STATIC
  CandidatesRepository.cpp
  Site.cpp
  Thing.cpp
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
  fronius
  goe
  shelly
  sunspec
)

target_include_directories(things
PUBLIC
  ../
)
