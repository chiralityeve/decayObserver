add_executable(fitter2 main.cpp Fitter.cpp)

target_link_libraries(fitter2 ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES})

