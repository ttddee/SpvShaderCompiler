
cmake_minimum_required (VERSION 3.8)

# glslang
set(LIB_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external)

add_subdirectory(${LIB_DIRECTORY}/glslang)

include_directories(${LIB_DIRECTORY}/glslang/glslang)

# Shader compiler static library
add_library (SpvShaderCompiler "SpvShaderCompiler.cpp" "SpvShaderCompiler.h")

target_link_libraries(SpvShaderCompiler glslang SPIRV)

# Copy header file to output directory
add_custom_command(TARGET SpvShaderCompiler POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/SpvShaderCompiler/SpvShaderCompiler.h $<TARGET_FILE_DIR:SpvShaderCompiler>)

