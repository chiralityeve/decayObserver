
# Specify .cpp files
set(SRC hello.cpp)

# Specify the executable
add_executable(FirstObservations-bin main.cpp ${SRC})
target_link_libraries(FirstObservations-bin ${LIBRARIES})
set_target_properties(FirstObservations-bin PROPERTIES OUTPUT_NAME FirstObservations)

# Build a library
add_library(FirstObservations ${SRC})
target_link_libraries(FirstObservations ${LIBRARIES})

# vim:ft=cmake


