# Executables
add_executable(Fitbkgr_insigregion fitbkgr_insigregion.cpp)
add_executable(Fitsignal_resonant fitsignal_resonant.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Fitbkgr_insigregion ${LIBRARIES})
target_link_libraries(Fitsignal_resonant ${LIBRARIES})
