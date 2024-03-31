CPMAddPackage(gh:Neargye/magic_enum@0.9.5)

add_library(uvw_net STATIC
  dns_sd/DnsMessage.cpp
  dns_sd/DnsParser.cpp
  dns_sd/DnsRecordDataSrv.cpp
)

target_include_directories(uvw_net
PUBLIC
  ${CMAKE_SOURCE_DIR}
PRIVATE
  ../
)

target_link_libraries(uvw_net
  magic_enum::magic_enum
)
