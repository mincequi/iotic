add_library(http
STATIC
  HttpClient.cpp
)

target_link_libraries(http
PRIVATE
  common
)

find_package(Boost REQUIRED)

target_include_directories(http
PRIVATE
  ${Boost_INCLUDE_DIRS}
)
