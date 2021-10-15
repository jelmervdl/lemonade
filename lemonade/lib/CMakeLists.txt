set(LIBRARY_SOURCES
   model_inventory.cpp
   translation_server.cpp
   translation_client.cpp
   translator.cpp
   utils.cpp
)

add_library(lemonade STATIC ${LIBRARY_SOURCES})
target_link_libraries(lemonade PUBLIC bergamot-translator marian fmt::fmt ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(lemonade PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})
