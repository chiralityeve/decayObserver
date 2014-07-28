add_executable(Hello hello.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Hello ${LIBRARIES})



add_subdirectory(Plotscript)
add_subdirectory(secateur)
add_subdirectory(fitter)

