add_executable(Say say.cpp)
add_library(Sayer Sayer.cpp)

target_link_libraries(Say Sayer ${ROOT_LIBRARIES})


