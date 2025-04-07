file(
 GLOB_RECURSE FINALPORTLALA_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE FINALPORTLALA_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this FINALPORTLALA's asset folder to bin when it is built
add_custom_target(copyAssetsFP ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${FINALPORTLALA_INC} DESTINATION include/FINALPORTLALA)
add_executable(FINALPORTLALA ${FINALPORTLALA_SRC} ${FINALPORTLALA_INC})
target_link_libraries(FINALPORTLALA PUBLIC core IMGUI assimp)
target_include_directories(FINALPORTLALA PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment0 is built
add_dependencies(FINALPORTLALA copyAssetsFP)