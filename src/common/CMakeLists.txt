set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS" CACHE INTERNAL "")

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON CACHE INTERNAL "Auto moc")
#set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Network SerialBus REQUIRED)

FetchContent_Declare(loguru
    GIT_REPOSITORY https://github.com/emilk/loguru.git
    GIT_TAG        v2.1.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(loguru)

#FetchContent_Declare(fmt
#  URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.tar.gz
#)
#if(NOT fmt_POPULATED)
#    FetchContent_Populate(fmt)
#    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()

FetchContent_Declare(magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.7.3
  GIT_PROGRESS TRUE
)
if(NOT magic_enum_POPULATED)
    FetchContent_Populate(magic_enum)
    add_subdirectory(${magic_enum_SOURCE_DIR} ${magic_enum_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(rpp
  URL https://github.com/victimsnino/ReactivePlusPlus/archive/refs/tags/v0.2.3.tar.gz
)
if(NOT rpp_POPULATED)
    FetchContent_Populate(rpp)
    add_subdirectory(${rpp_SOURCE_DIR} ${rpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(BUILD_UVW_LIBS ON)
FetchContent_Declare(uvw
  URL https://github.com/skypjack/uvw/archive/refs/tags/v3.2.0_libuv_v1.46.tar.gz
)
if(NOT uvw_POPULATED)
    FetchContent_Populate(uvw)
    add_subdirectory(${uvw_SOURCE_DIR} ${uvw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(common STATIC
  Logger.cpp
  Util.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
)

target_include_directories(common
PUBLIC
  ../
  ${loguru_SOURCE_DIR}
PRIVATE
  src
)

target_link_libraries(common
PUBLIC
  ${CMAKE_DL_LIBS}
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialBus
  #fmt::fmt
  magic_enum
  rpp
  rppqt
  uvw::uvw-static
)
