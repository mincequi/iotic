add_library(fronius STATIC
  FroniusDiscovery.cpp
)

target_link_libraries(fronius
PUBLIC
  things
)
