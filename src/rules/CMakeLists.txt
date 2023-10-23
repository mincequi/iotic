include(FetchContent)
FetchContent_Declare(exprtk
    URL    https://github.com/ArashPartow/exprtk/archive/refs/tags/0.0.2.tar.gz
)
FetchContent_MakeAvailable(exprtk)
if(NOT exprtk_POPULATED)
    FetchContent_Populate(exprtk)
    add_subdirectory(${exprtk_SOURCE_DIR} ${exprtk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(rules STATIC
  GenericActuationStrategy.cpp
  StrategyFactory.cpp
  RulesEngine.cpp
  #test_schedulers.cpp
)

target_include_directories(rules
PUBLIC
  ../
  ${exprtk_SOURCE_DIR}
)

target_link_libraries(rules
PRIVATE
  common
  config
  strategies
)
