include(FetchContent)

FetchContent_Declare(tinyexpr
    GIT_REPOSITORY    https://github.com/Blake-Madden/tinyexpr-plusplus.git
    GIT_TAG           f0038215766c79e295d9bd6daa8da7c15a8e952b
)
FetchContent_MakeAvailable(tinyexpr)

add_library(rules STATIC
  RuleActuationStrategy.cpp
  RulesEngine.cpp
  StrategyFactory.cpp
  ${tinyexpr_SOURCE_DIR}/tinyexpr.cpp
)

target_include_directories(rules
PUBLIC
  ../
  ${tinyexpr_SOURCE_DIR}
)

target_link_libraries(rules
PRIVATE
  common
  config
  strategies
)
