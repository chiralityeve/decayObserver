# Executables
add_executable(d_Plotscript d_Plotscript.cpp Plotvariable.cpp tree2hist.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(d_Plotscript ${LIBRARIES})

