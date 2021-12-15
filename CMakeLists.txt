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
  loguru
  GIT_REPOSITORY https://github.com/emilk/loguru.git
  GIT_TAG        v2.1.0
)
FetchContent_MakeAvailable(loguru)

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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-DLOGURU_WITH_STREAMS -DLOGURU_FILENAME_WIDTH=15")
set(CMAKE_EXE_LINKER_FLAGS "-lpthread -ldl")

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
  src/sunspec/SunSpecCommonModel.cpp
  src/sunspec/SunSpecDataPoint.cpp
  src/sunspec/SunSpecDataValue.cpp
  src/sunspec/SunSpecInverterModelFactory.cpp
  src/sunspec/SunSpecManager.cpp
  src/sunspec/SunSpecMeasuredValue.cpp
  src/sunspec/SunSpecModel.cpp
  src/sunspec/SunSpecModelFactory.cpp
  src/sunspec/SunSpecMpptInverterExtensionModelFactory.cpp
  src/sunspec/SunSpecStatsModel.cpp
  src/sunspec/SunSpecThing.cpp
  src/sunspec/SunSpecWyeConnectMeterModelFactory.cpp
  ${loguru_SOURCE_DIR}/loguru.cpp
)

target_include_directories(elsewhere
PRIVATE
  ${loguru_SOURCE_DIR}
)

target_link_libraries(elsewhere
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialBus
  magic_enum
  qmqtt
  tomlplusplus::tomlplusplus
)
