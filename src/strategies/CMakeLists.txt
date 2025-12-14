add_library(strategies STATIC
  EvseStrategy.h
  EvseStrategy.cpp
  Strategy.h
  Strategy.cpp
  StrategyRepository.h
  StrategyRepository.cpp
  PowerManager.h
  PowerManager.cpp
  ThreePhaseStrategy.h
  ThreePhaseStrategy.cpp
)

target_link_libraries(strategies
PRIVATE
  common
  config
)
