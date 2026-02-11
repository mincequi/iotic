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

cmrc_add_resource_library(webapp WHENCE web
  web/favicon.png
  web/flutter.js
  web/flutter_service_worker.js
  web/index.html
  web/main.dart.js
  web/manifest.json
  web/assets/FontManifest.json
  web/assets/fonts/MaterialIcons-Regular.otf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Black.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Bold.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Light.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Medium.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Regular.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Thin.ttf
  web/icons/Icon-192.png
)

#add_custom_command(TARGET webapp
#  PRE_BUILD
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND flutter build web --web-renderer html --no-tree-shake-icons
#)
