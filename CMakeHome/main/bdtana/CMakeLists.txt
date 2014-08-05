add_executable(bdtana bdtana.cpp BdtAnalyser.cpp)

target_link_libraries(bdtana ${ROOT_LIBRARIES} TMVA)

