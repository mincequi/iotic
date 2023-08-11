set(FETCHCONTENT_QUIET FALSE)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(BUILD_TESTS OFF)

FetchContent_Declare(qmdns
    GIT_REPOSITORY https://github.com/nitroshare/qmdnsengine.git
    GIT_TAG        0ca80117e853671d909b3cec9e2bdcac85a13b9f # main from Jan 16, 2022
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(qmdns)

add_library(http STATIC
  HttpDiscovery.cpp
  HttpThing.cpp
)

target_link_libraries(http
PUBLIC
  common
  qmdnsengine
)

#target_include_directories(http
#PUBLIC
#  ../
#)