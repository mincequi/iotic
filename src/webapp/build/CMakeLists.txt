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
  web/flutter_bootstrap.js
  web/version.json
  web/.last_build_id
  web/favicon.ico
  web/index.html
  web/apple-touch-icon.png
  web/main.dart.js
  web/flutter.js
  web/flutter_service_worker.js
  web/favicon.png
  web/icons/Icon-192.png
  web/icons/Icon-maskable-192.png
  web/icons/Icon-maskable-512.png
  web/icons/Icon-512.png
  web/apple-touch-icon-precomposed.png
  web/manifest.json
  web/apple-touch-icon-120x120-precomposed.png
  web/assets/NOTICES
  web/assets/FontManifest.json
  web/assets/AssetManifest.bin.json
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Medium.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Light.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Regular.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Bold.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Thin.ttf
  web/assets/packages/icon_decoration/assets/fonts/Roboto/Roboto-Black.ttf
  web/assets/shaders/ink_sparkle.frag
  web/assets/shaders/stretch_effect.frag
  web/assets/AssetManifest.bin
  web/assets/fonts/MaterialIcons-Regular.otf
  # Apparently we don't need to incldue canvaski. This reduces the size of the binary by a lot.
  #web/canvaskit/skwasm.js
  #web/canvaskit/skwasm_heavy.js
  #web/canvaskit/skwasm.js.symbols
  #web/canvaskit/canvaskit.js.symbols
  #web/canvaskit/skwasm_heavy.js.symbols
  #web/canvaskit/skwasm.wasm
  #web/canvaskit/chromium/canvaskit.js.symbols
  #web/canvaskit/chromium/canvaskit.js
  #web/canvaskit/chromium/canvaskit.wasm
  #web/canvaskit/canvaskit.js
  #web/canvaskit/canvaskit.wasm
  #web/canvaskit/skwasm_heavy.wasm
)

#add_custom_command(TARGET webapp
#  PRE_BUILD
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND flutter build web --web-renderer html --no-tree-shake-icons
#)
