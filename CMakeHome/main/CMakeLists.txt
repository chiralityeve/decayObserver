add_executable(Hello hello.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Hello ${LIBRARIES})



add_subdirectory(Plotscript)
add_subdirectory(secateur)
add_subdirectory(fitter)
add_subdirectory(fitter2)
add_subdirectory(bdtana)
add_subdirectory(Getlumi)

