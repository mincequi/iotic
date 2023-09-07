include(FetchContent)
FetchContent_Declare(exprtk
    URL    https://github.com/ArashPartow/exprtk/archive/refs/tags/0.0.2.tar.gz
)
FetchContent_MakeAvailable(exprtk)

add_library(rules STATIC
  OnOffRule.cpp
  Rule.cpp
  RuleFactory.cpp
  RulesEngine.cpp
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
)
