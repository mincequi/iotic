set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS -DLOGURU_USE_FMTLIB" CACHE INTERNAL "")

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

FetchContent_Declare(fmt
  URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.tar.gz
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.7.3
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(rpp
  URL https://github.com/victimsnino/ReactivePlusPlus/archive/refs/tags/v0.2.3.tar.gz
)
FetchContent_MakeAvailable(rpp)

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
  fmt::fmt
  magic_enum
  rpp
  rppqt
)
