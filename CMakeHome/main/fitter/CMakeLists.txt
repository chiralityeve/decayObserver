add_executable(fitter main.cpp Fitter.cpp FitterPdf.cpp)

target_link_libraries(fitter ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES})

