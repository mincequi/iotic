set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(qmqtt_SHARED OFF)
add_definitions(-DMQTT_PROJECT_INCLUDE_SRC)
FetchContent_Declare(qmqtt
  #GIT_REPOSITORY https://github.com/emqx/qmqtt.git
  #GIT_TAG        v1.0.3
  # TODO: For Qt6 we need a custom patch
  GIT_REPOSITORY https://github.com/mincequi/qmqtt.git
  GIT_TAG        7724c938690c8ce40b56b068cd98cffb0d667a82
  GIT_PROGRESS TRUE
)
# This is needed to exclude .h and .a files from installation
if(NOT qmqtt_POPULATED)
    FetchContent_Populate(qmqtt)
    add_subdirectory(${qmqtt_SOURCE_DIR} ${qmqtt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(mqtt STATIC
  MqttExporter.cpp
)

target_link_libraries(mqtt
PUBLIC
  common
  qmqtt
)
