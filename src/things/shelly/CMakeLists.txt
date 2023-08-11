add_library(shelly STATIC
  Shelly.cpp
)

target_link_libraries(shelly
PUBLIC
  http
)
