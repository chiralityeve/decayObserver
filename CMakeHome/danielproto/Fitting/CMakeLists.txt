# Executables
add_executable(Fitbkgr_insigregion fitbkgr_insigregion.cpp)
add_executable(Fitsignal_resonant fitsignal_resonant.cpp)
add_executable(FitBKGRafterBDT_punzi FitBKGRafterBDT_punzi.cpp)
add_executable(FitBKGRafterBDT_common FitBKGRafterBDT_common.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Fitbkgr_insigregion ${LIBRARIES})
target_link_libraries(Fitsignal_resonant ${LIBRARIES})
target_link_libraries(FitBKGRafterBDT_punzi ${LIBRARIES})
target_link_libraries(FitBKGRafterBDT_common ${LIBRARIES})
