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
PUBLIC
  common
PRIVATE
  config
)

target_include_directories(strategies
PUBLIC
  ../
)
