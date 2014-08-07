
add_executable(secateur secateur.cpp SelectDialog.cpp SelectDialogDict.cpp)

set_source_files_properties(SelectDialogDict.cpp PROPERTIES GENERATED TRUE)

target_link_libraries(secateur ${ROOT_LIBRARIES})	

# Create the dictionary files for ROOT
add_custom_target(
	SelectDialogDict
	
	# Create a dummy pcm file for if we are not using ROOT 6
	COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/SelectDialogDict_rdict.pcm"
	
	# Call rootcint
	COMMAND rootcint -f "${CMAKE_CURRENT_BINARY_DIR}/SelectDialogDict.cpp" -c "${CMAKE_CURRENT_SOURCE_DIR}/SelectDialog.h" "${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h"
)

add_custom_command(
	OUTPUT SelectDialogDict.cpp
	DEPENDS SelectDialogDict
)

# Copy the created pcm file to the binary directory. (Required for ROOT 6)
add_custom_command(
	TARGET secateur POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/SelectDialogDict_rdict.pcm" "$<TARGET_FILE_DIR:secateur>"
)
