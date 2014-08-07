# Executables
add_executable(Plotscript Plotscript.cpp daniel_settings.cpp
    vitali_settings.cpp kevin_settings.cpp Plotvariable.cpp
    Plotvariable_2D.cpp tree2hist.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Plotscript ${ROOT_LIBRARIES})

