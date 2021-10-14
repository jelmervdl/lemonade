
set(IBUS_BUS_NAME "org.freedesktop.IBus.Lemonade")
set(IBUS_COMMANDLINE "")
set(IBUS_TEXTDOMAIN "lemonade-ime")
set(IBUS_ICON "/usr/share/icons/lemonade.svg")
set(IBUS_LAYOUT "us")
set(IBUS_LANGUAGE "en")


find_package(PkgConfig)
PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lemonade.xml.in
               ${CMAKE_CURRENT_BINARY_DIR}/lemonade.xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/application.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/application.h @ONLY)


## Create your Executable and link it. `.....` your sources
add_executable(lemonade-ime main.cpp engine-compat.cpp)
## `....` place your created libraries
target_link_libraries(lemonade-ime PUBLIC ${GLIB2_LIBRARIES} ${IBUS_LIBRARIES} )
target_include_directories(lemonade-ime PRIVATE ${GLIB2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS})

