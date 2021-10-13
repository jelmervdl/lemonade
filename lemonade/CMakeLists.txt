
set(INCLUDE_DIRECTORIES 
    ${CMAKE_SOURCE_DIR}/3rd_party/CLI
    ${CMAKE_SOURCE_DIR}/3rd_party/rapidjson/include
)

add_library(lemonade STATIC model_inventory.cpp)
target_link_libraries(lemonade PUBLIC bergamot-translator marian ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(lemonade PRIVATE ${INCLUDE_DIRECTORIES})

add_executable(lemonade-bin main.cpp )
target_link_libraries(lemonade-bin PRIVATE lemonade)
target_include_directories(lemonade-bin PRIVATE ${INCLUDE_DIRECTORIES})
