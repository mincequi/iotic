add_library(strategies STATIC
  EvseStrategy.cpp
  Strategy.cpp
)

target_link_libraries(strategies
PRIVATE
  common
  config
)
