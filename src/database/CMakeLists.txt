CPMAddPackage("gh:erthink/libmdbx@0.13.11")

add_library(database STATIC
  Database.cpp
  DatabaseUtil.h DatabaseUtil.cpp
  DatabaseTypes.h
  ArchiveDayUseCase.h ArchiveDayUseCase.cpp
)

target_link_libraries(database
PUBLIC
  uvw_iot # needs to be public for testing
PRIVATE
  common
  mdbx-static
)

target_include_directories(database
PUBLIC
  ../
)
