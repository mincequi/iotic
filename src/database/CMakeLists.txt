CPMAddPackage("gh:erthink/libmdbx@0.13.11")

add_library(database STATIC
  Database.cpp
)

target_link_libraries(database
PRIVATE
  mdbx-static
  uvw_iot
)

target_include_directories(database
PUBLIC
  ../
)
