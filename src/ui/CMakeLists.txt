#cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(BUILD_EXAMPLES OFF CACHE INTERNAL "Build examples")
set(INSTALL_DOCUMENTATION OFF CACHE INTERNAL "Install documentation (tutorials and reference)")
set(INSTALL_EXAMPLES OFF CACHE INTERNAL "Install examples (binaries and source)")
set(INSTALL_RESOURCES ON CACHE INTERNAL "Install resources directory")
set(ENABLE_SSL ON CACHE INTERNAL "Enable cryptography functions, using OpenSSL")
set(ENABLE_HARU OFF CACHE INTERNAL "Enable Haru Free PDF Library, which is used to provide support for painting to PDF (WPdfImage)")
set(ENABLE_PANGO ON CACHE INTERNAL "Enable Pango Library, which is used for improved font support (WPdfImage and WRasterImage)")
set(ENABLE_EXT OFF CACHE INTERNAL "Build Wt Ext library with JavaScript-only widgets (http://extjs.com/) DEPRECATED")
set(ENABLE_SQLITE OFF CACHE INTERNAL "Build SQLite3 backend for Wt::Dbo")
set(ENABLE_POSTGRES OFF CACHE INTERNAL "Build PostgreSQL backend for Wt::Dbo")
set(ENABLE_FIREBIRD OFF CACHE INTERNAL "Build FirebirdSQL backend for Wt::Dbo")
set(ENABLE_MYSQL OFF CACHE INTERNAL "Build mariadb/mysql backend for Wt::Dbo")
set(ENABLE_MSSQLSERVER OFF CACHE INTERNAL "Build Microsoft SQL Server backend for Wt::Dbo")
set(ENABLE_QT4 OFF CACHE INTERNAL "Build Qt4 interworking library (libwtwithqt)")
set(ENABLE_QT5 ON CACHE INTERNAL "Build Qt5 interworking library (libwtwithqt5)")
set(ENABLE_QT6 ON CACHE INTERNAL "Build Qt6 interworking library (libwtwithqt6)")
set(ENABLE_SAML OFF CACHE INTERNAL "Build built-in SAML service provider for Wt::Auth")
set(ENABLE_LIBWTTEST OFF CACHE INTERNAL "Build Wt::Test")
set(ENABLE_LIBWTDBO OFF CACHE INTERNAL "Build Wt::Dbo")
set(WT_NO_STD_LOCALE OFF CACHE INTERNAL "Build Wt to run on a system without std::locale support")
set(WT_NO_STD_WSTRING OFF CACHE INTERNAL "Build Wt to run on a system without std::wstring support")
set(ENABLE_OPENGL ON CACHE INTERNAL "Build Wt with support for server-side opengl rendering")
set(ENABLE_UNWIND OFF CACHE INTERNAL "Build Wt with stacktrace support using libunwind")

FetchContent_Declare(witty
  URL https://github.com/emweb/wt/archive/4.8.2.tar.gz
)
# This is needed to exclude .h and .a files from installation
#if(NOT witty_POPULATED)
#    FetchContent_Populate(witty)
    #add_subdirectory(${witty_SOURCE_DIR} ${witty_BINARY_DIR} EXCLUDE_FROM_ALL)
    #add_subdirectory(${witty_SOURCE_DIR} ${witty_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()
FetchContent_MakeAvailable(witty)

QT5_GENERATE_MOC(
  ${witty_SOURCE_DIR}/examples/wtwithqt/lib/DispatchThread.h
  ${witty_BINARY_DIR}/mocDispatchThread5.C
)

add_library(ui STATIC
  UiExport.cpp
  ${witty_SOURCE_DIR}/examples/wtwithqt/lib/DispatchThread.h
  ${witty_SOURCE_DIR}/examples/wtwithqt/lib/DispatchThread.C
  ${witty_SOURCE_DIR}/examples/wtwithqt/lib/WQApplication.C
  ${witty_BINARY_DIR}/mocDispatchThread5.C
)

target_include_directories(ui
PUBLIC
  ${witty_SOURCE_DIR}/examples/wtwithqt/lib
  ${witty_SOURCE_DIR}/src
  ${witty_BINARY_DIR}
)

target_link_libraries(ui
PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  wt
  wthttp
)


