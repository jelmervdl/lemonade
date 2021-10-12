
add_executable(lemonade main.cpp)

set(LINK_LIBRARIES
        bergamot-translator 
        ${CMAKE_THREAD_LIBS_INIT} 
        ${CMAKE_DL_LIBS}
)
target_link_libraries(lemonade PRIVATE ${LINK_LIBRARIES})

target_include_directories(lemonade PRIVATE
    ${CMAKE_SOURCE_DIR}/3rd_party/CLI
    ${CMAKE_SOURCE_DIR}/3rd_party/rapidjson/include
)

