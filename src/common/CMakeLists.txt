set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS -DLOGURU_USE_FMTLIB" CACHE INTERNAL "")

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON CACHE INTERNAL "Auto moc")
#set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 Qt6 COMPONENTS Core Network SerialBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network SerialBus REQUIRED)

FetchContent_Declare(loguru
    GIT_REPOSITORY https://github.com/emilk/loguru.git
    GIT_TAG        v2.1.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(loguru)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        8.0.1
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        v0.7.3
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(rpp
    GIT_REPOSITORY https://github.com/victimsnino/ReactivePlusPlus.git
    GIT_TAG       a9a6898ed15aead18bce1c2659ea3aae4c6fa717
    #GIT_TAG       e90868f0eb57b3ce9a1b2e0102257161b3df1b7c
    #GIT_TAG       8b73c31183b9a919cd8284431fbb22de6aec8417
    GIT_PROGRESS TRUE
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
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialBus
    fmt::fmt
    influx
    magic_enum
    rpp
    rppqt
)
