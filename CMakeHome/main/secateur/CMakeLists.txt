add_executable(secateur secateur.cpp SelectDialog.cpp SelectDialogDict.cpp)

target_link_libraries(secateur ${ROOT_LIBRARIES})	

add_custom_command(TARGET secateur POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/SelectDialogDict_rdict.pcm" "$<TARGET_FILE_DIR:secateur>")