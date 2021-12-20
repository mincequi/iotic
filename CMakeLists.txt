cmake_minimum_required(VERSION 3.14)

project(elsewhere LANGUAGES CXX)

Include(FetchContent)

#FetchContent_Declare(
#  Catch2
#  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#  GIT_TAG        v3.0.0-preview3
#)
#FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v2.5.0
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        8.0.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  loguru
  GIT_REPOSITORY https://github.com/emilk/loguru.git
  GIT_TAG        v2.1.0
)
FetchContent_MakeAvailable(loguru)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(qmqtt_SHARED OFF)
add_definitions(-DMQTT_PROJECT_INCLUDE_SRC)
FetchContent_Declare(
  qmqtt
  GIT_REPOSITORY https://github.com/emqx/qmqtt.git
  GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(qmqtt)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.7.3
)
FetchContent_MakeAvailable(magic_enum)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS -DLOGURU_USE_FMTLIB")

find_package(Threads)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network SerialBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network SerialBus REQUIRED)

add_executable(elsewhere
  src/main.cpp
  src/LiveExporter.cpp
  src/Logger.cpp
  src/MqttExporter.cpp
  src/Statistics.cpp
  src/Util.cpp
  src/sunspec/SunSpecBlock.cpp
  src/sunspec/SunSpecDataPoint.cpp
  src/sunspec/SunSpecDataValue.cpp
  src/sunspec/SunSpecManager.cpp
  src/sunspec/SunSpecMeasuredValue.cpp
  src/sunspec/SunSpecModel.cpp
  src/sunspec/SunSpecModelFactory.cpp
  src/sunspec/SunSpecStatsModel.cpp
  src/sunspec/SunSpecThing.cpp
  src/sunspec/SunSpecTypes.cpp
  src/sunspec/models/SunSpecCommonModel.cpp
  src/sunspec/models/SunSpecElgrisSmartMeterModelFactory.cpp
  src/sunspec/models/SunSpecInverterModelFactory.cpp
  src/sunspec/models/SunSpecMpptInverterExtensionModelFactory.cpp
  src/sunspec/models/SunSpecWyeConnectMeterModelFactory.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
)

target_include_directories(elsewhere
PRIVATE
  src
  ${loguru_SOURCE_DIR}
)

target_link_libraries(elsewhere
  Threads::Threads
  ${CMAKE_DL_LIBS}
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialBus
  magic_enum
  qmqtt
  fmt::fmt
  tomlplusplus::tomlplusplus
)
