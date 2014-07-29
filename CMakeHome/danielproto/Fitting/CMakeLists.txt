# Executables
add_executable(Fit fit.cpp)
add_executable(FitwSelection fitselection.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Plotscript ${LIBRARIES})

