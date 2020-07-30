
########################################################################
# If the build fails with a "runtime mismatch" error in Visual Studio, 
# make sure that gtest_force_shared_crt:BOOL=ON is set in CMakeCache.txt
########################################################################

cmake_minimum_required (VERSION 3.8)

set(LIB_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external)

add_subdirectory(${LIB_DIRECTORY}/googletest)

include_directories(${LIB_DIRECTORY}/googletest/googletest)

set(Sources
	"SpvShaderCompilerTests.cpp"
)

add_executable(SpvShaderCompilerTests ${Sources})

target_link_libraries(SpvShaderCompilerTests PUBLIC
	gtest_main
	SpvShaderCompiler
)

add_test(
	NAME SpvShaderCompilerTests
	COMMAND SpvShaderCompilerTests
)


# Copy shaders to output directory for testing
add_custom_command(TARGET SpvShaderCompilerTests PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/SpvShaderCompilerTests/shaders/blur.comp $<TARGET_FILE_DIR:SpvShaderCompilerTests>)

add_custom_command(TARGET SpvShaderCompilerTests PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/SpvShaderCompilerTests/shaders/blur_comp.spv $<TARGET_FILE_DIR:SpvShaderCompilerTests>)

add_custom_command(TARGET SpvShaderCompilerTests PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/SpvShaderCompilerTests/shaders/blur_errors.comp $<TARGET_FILE_DIR:SpvShaderCompilerTests>)