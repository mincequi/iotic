add_library(influx STATIC
  InfluxExporter.cpp
)

find_package(CURL)
if(CURL_FOUND)
  set(BUILD_SHARED_LIBS OFF)
  set(INFLUXCXX_WITH_BOOST OFF)
  set(INFLUXCXX_TESTING OFF)
  set(INFLUXCXX_SYSTEMTEST OFF)
  FetchContent_Declare(
    influxdb
    GIT_REPOSITORY https://github.com/offa/influxdb-cxx.git
    #GIT_TAG        v0.6.7  # This one does not compile due to boost error
    #GIT_TAG        v0.7.1 (and v0.7.0) require libcrp which is not available on debian/ubuntu
    GIT_TAG        bdb4859bb90cc3eca7c812fab7987814a7483782
  )
  # This is needed to exclude .h and .a files from installation
  if(NOT influxdb_POPULATED)
    FetchContent_Populate(influxdb)
    add_subdirectory(${influxdb_SOURCE_DIR} ${influxdb_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()

  target_link_libraries(influx
  PUBLIC
    InfluxData::InfluxDB
  )
  add_compile_definitions(
    USE_INFLUXDB
  )
endif(CURL_FOUND)

target_link_libraries(influx
PUBLIC
  common
)
