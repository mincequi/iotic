find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS -DLOGURU_USE_FMTLIB" CACHE INTERNAL "")

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
    fmt::fmt
    magic_enum
)
