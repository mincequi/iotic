add_library(sunspec STATIC
  SunSpecBlock.cpp
  SunSpecDataPoint.cpp
  SunSpecLogger.cpp
  SunSpecManager.cpp
  SunSpecMeasuredValue.cpp
  SunSpecModel.cpp
  SunSpecModelFactory.cpp
  SunSpecStatsModel.cpp
  SunSpecStatsValue.cpp
  SunSpecThing.cpp
  SunSpecTypes.cpp
  models/SunSpecCommonModelFactory.cpp
  models/SunSpecElgrisSmartMeterModelFactory.cpp
  models/SunSpecInverterModelFactory.cpp
  models/SunSpecMpptInverterExtensionModelFactory.cpp
  models/SunSpecWyeConnectMeterModelFactory.cpp
)

target_link_libraries(sunspec
PRIVATE
  modbus
)
