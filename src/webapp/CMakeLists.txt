include(FetchContent)
FetchContent_Declare(cmakerc
  GIT_REPOSITORY    https://github.com/vector-of-bool/cmrc.git
  GIT_TAG           952ffddba731fc110bd50409e8d2b8a06abbd237
)
if(NOT cmakerc_POPULATED)
    FetchContent_Populate(cmakerc)
    add_subdirectory(${cmakerc_SOURCE_DIR} ${cmakerc_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include(${cmakerc_SOURCE_DIR}/CMakeRC.cmake)

#find_program(FLUTTER flutter)
#if(NOT FLUTTER)
#  message(FATAL_ERROR "Flutter not found!")
#endif()

#execute_process(
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND flutter build web --web-renderer html --no-tree-shake-icons
#)

file(GLOB_RECURSE webapp_src
  "build/web/*.bin"
  "build/web/*.frag"
  "build/web/*.html"
  "build/web/*.ico"
  "build/web/*.js"
  "build/web/*.json"
  "build/web/*.otf"
  "build/web/*.png"
  "build/web/*.wasm"
)

cmrc_add_resource_library(webapp WHENCE build/web
  ${webapp_src}
)

#add_custom_command(TARGET webapp
#  PRE_BUILD
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND flutter build web --web-renderer html --no-tree-shake-icons
#)
