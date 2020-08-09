cmake_minimum_required (VERSION 3.8)

# glslang
set(LIB_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/glslang/install/lib)

set(INC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external)

include_directories(${INC_DIRECTORY}/glslang/install/include)

set(Headers
    "SpvShaderCompiler.h"
)

set(Sources
    "SpvShaderCompiler.cpp"
)

add_library (SpvShaderCompiler STATIC ${Sources} ${Headers})

if(WIN32)
    target_link_libraries(SpvShaderCompiler PUBLIC ${LIB_DIRECTORY}/glslang.lib 
    ${LIB_DIRECTORY}/SPIRV.lib ${LIB_DIRECTORY}/MachineIndependent.lib 
    ${LIB_DIRECTORY}/OSDependent.lib ${LIB_DIRECTORY}/GenericCodeGen.lib 
    ${LIB_DIRECTORY}/HLSL.lib ${LIB_DIRECTORY}/OGLCompiler.lib 
    ${LIB_DIRECTORY}/SPVRemapper.lib)
endif()

# Copy header file to output directory
add_custom_command(TARGET SpvShaderCompiler POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/SpvShaderCompiler/SpvShaderCompiler.h $<TARGET_FILE_DIR:SpvShaderCompiler>)

