add_library(goe STATIC
  GoeCharger.cpp
)

target_link_libraries(goe
PUBLIC
  things
  http
)
