configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bergamot.xml.in
               ${CMAKE_CURRENT_BINARY_DIR}/bergamot.xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/application.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/application.h @ONLY)


## Create your Executable and link it. `.....` your sources
add_executable(lemonade-ime main.cpp engine-compat.cpp)
## `....` place your created libraries
target_link_libraries(lemonade-ime PUBLIC ${GLIB2_LIBRARIES} ${IBUS_LIBRARIES} )
target_include_directories(lemonade-ime PRIVATE ${GLIB2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS})

