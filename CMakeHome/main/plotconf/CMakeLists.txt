# Executables
add_executable(plotconf plotconf.cpp Plotvariable.cpp
    Plotvariable_2D.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(plotconf ${ROOT_LIBRARIES})

