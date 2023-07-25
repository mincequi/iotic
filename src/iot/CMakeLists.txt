set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(BUILD_TESTS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network SerialBus REQUIRED)

FetchContent_Declare(qmdns
    GIT_REPOSITORY https://github.com/nitroshare/qmdnsengine.git
    GIT_TAG        0ca80117e853671d909b3cec9e2bdcac85a13b9f # main from Jan 16, 2022
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(qmdns)

add_library(iot STATIC
    AbstractDiscovery.cpp
    AbstractThing.cpp
    ThingFactory.cpp
    ThingInfo.cpp
    ThingManager.cpp
    http/HttpDiscovery.cpp
    http/HttpThing.cpp
    goe/GoeCharger.cpp
    sunspec/SunSpecBlock.cpp
    sunspec/SunSpecDataPoint.cpp
    sunspec/SunSpecDataValue.cpp
    sunspec/SunSpecLogger.cpp
    sunspec/SunSpecManager.cpp
    sunspec/SunSpecMeasuredValue.cpp
    sunspec/SunSpecModel.cpp
    sunspec/SunSpecModelFactory.cpp
    sunspec/SunSpecStatsModel.cpp
    sunspec/SunSpecThing.cpp
    sunspec/SunSpecTypes.cpp
    sunspec/models/SunSpecCommonModelFactory.cpp
    sunspec/models/SunSpecElgrisSmartMeterModelFactory.cpp
    sunspec/models/SunSpecInverterModelFactory.cpp
    sunspec/models/SunSpecMpptInverterExtensionModelFactory.cpp
    sunspec/models/SunSpecWyeConnectMeterModelFactory.cpp
)

target_link_libraries(iot
PUBLIC
    #Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::SerialBus
    common
    qmdnsengine
)
