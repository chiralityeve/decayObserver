# Executables
add_executable(2D-Fit_Resonant_selection_punzi 2D-Fit_Resonant_selection_punzi.cpp)
add_executable(2D-Fit_Non-Resonant_selection_punzi 2D-Fit_Non-Resonant_selection_punzi.cpp)


# Aliases for module libs

# Linker information
target_link_libraries(2D-Fit_Resonant_selection_punzi ${LIBRARIES})
target_link_libraries(2D-Fit_Non-Resonant_selection_punzi ${LIBRARIES})

