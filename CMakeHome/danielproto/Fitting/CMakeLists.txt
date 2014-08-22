# Executables
add_executable(Fitbkgr_insigregion fitbkgr_insigregion.cpp)
add_executable(Fitsignal_resonant fitsignal_resonant.cpp)
add_executable(Fitsignal_resonant_CB fitsignal_resonant_CB.cpp)
add_executable(FitBKGRafterBDT_punzi FitBKGRafterBDT_punzi.cpp)
add_executable(FitBKGRafterBDT_common FitBKGRafterBDT_common.cpp)
add_executable(FitResSignalafterBDT_common FitResSignalafterBDT_common.cpp)
add_executable(FitResSignalafterBDT_punzi FitResSignalafterBDT_punzi.cpp)

# Aliases for module libs

# Linker information
target_link_libraries(Fitbkgr_insigregion ${LIBRARIES})
target_link_libraries(Fitsignal_resonant ${LIBRARIES})
target_link_libraries(Fitsignal_resonant_CB ${LIBRARIES})
target_link_libraries(FitBKGRafterBDT_punzi ${LIBRARIES})
target_link_libraries(FitBKGRafterBDT_common ${LIBRARIES})
target_link_libraries(FitResSignalafterBDT_common ${LIBRARIES})
target_link_libraries(FitResSignalafterBDT_punzi ${LIBRARIES})
