# Executables
add_executable(Plotscript Plotscript.cpp Plotvariable.cpp tree2hist.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Plotscript ${LIBRARIES})

