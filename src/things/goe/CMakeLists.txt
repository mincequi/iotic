add_library(goe STATIC
  GoeCharger.cpp
  GoeUtil.cpp
)

target_link_libraries(goe
PUBLIC
  things
  http
)
