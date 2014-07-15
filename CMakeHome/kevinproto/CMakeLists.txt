add_executable(kevinproto main.cpp)

target_link_libraries(kevinproto ${ROOT_LIBRARIES})

# Daniel's plot script
add_subdirectory(Plotscript)

# Pruning root trees
add_subdirectory(secateur)


