set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(things STATIC
  ThingDiscovery.cpp
  ThingInfo.cpp
  ThingValue.cpp
  ThingsManager.cpp
)

target_link_libraries(things
PUBLIC
  common
  config
  #fronius
)

target_include_directories(things
PUBLIC
  ../
)
