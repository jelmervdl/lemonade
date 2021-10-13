
set(INCLUDE_DIRECTORIES 
    ${CMAKE_SOURCE_DIR}/3rd_party/CLI
    ${CMAKE_SOURCE_DIR}/3rd_party/rapidjson/include
    ${CMAKE_SOURCE_DIR}
)

add_library(lemonade STATIC model_inventory.cpp)
target_link_libraries(lemonade PUBLIC bergamot-translator marian fmt::fmt ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(lemonade PRIVATE ${INCLUDE_DIRECTORIES})



add_executable(lemonade-bin main.cpp )
target_link_libraries(lemonade-bin PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-bin PRIVATE ${INCLUDE_DIRECTORIES})

add_executable(lemonade-server server.cpp )
target_link_libraries(lemonade-server PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-server PRIVATE ${INCLUDE_DIRECTORIES})

add_executable(lemonade-client client.cpp )
target_link_libraries(lemonade-client PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-client PRIVATE ${INCLUDE_DIRECTORIES})
