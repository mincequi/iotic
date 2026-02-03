add_library(strategies STATIC
  WallboxStrategy.h
  WallboxStrategy.cpp
  Strategy.h
  Strategy.cpp
  StrategyRepository.h
  StrategyRepository.cpp
  PowerManager.h
  PowerManager.cpp
  MultiPhaseStrategy.h
  MultiPhaseStrategy.cpp
)

target_link_libraries(strategies
PRIVATE
  common
  config
)
