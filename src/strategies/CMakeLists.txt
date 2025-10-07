add_library(strategies STATIC
  EvseStrategy.cpp
  Strategy.cpp
  StrategyRepository.h
  StrategyRepository.cpp
  PowerManager.h
  PowerManager.cpp
)

target_link_libraries(strategies
PRIVATE
  common
  config
)
