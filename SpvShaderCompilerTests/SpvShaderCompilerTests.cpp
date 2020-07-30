#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../SpvShaderCompiler/SpvShaderCompiler.h"

struct SpvShaderCompilerTests : public ::testing::Test
{
    SpvCompiler compiler;
};

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file.");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> fileBuffer(fileSize);

    file.seekg(0);
    file.read(fileBuffer.data(), fileSize);
    file.close();

    return fileBuffer;
}

std::vector<char> uintVecToCharVec(const std::vector<unsigned int>& in)
{
    std::vector<char> out;

    for (size_t i = 0; i < in.size(); i++)
    {
        out.push_back(in[i] >> 0);
        out.push_back(in[i] >> 8);
        out.push_back(in[i] >> 16);
        out.push_back(in[i] >> 24);
    }

    return out;
}

void writeCharVecToBinary(const std::vector<char>& in, const std::string& filename)
{
    std::ofstream fout(filename, std::ios::out | std::ios::binary);
    fout.write((char*)&in[0], in.size());
    fout.close();
}

///////////////////////
// SPVCOMPILER TESTS //
///////////////////////

TEST_F(SpvShaderCompilerTests, compileGLSLFromFile_CompareToOutputOfGlslangValidator)
{
    // Load and compile shader
    bool result = compiler.compileGLSLFromFile("blur.comp");
    auto spirvCompiled = compiler.getSpirV();
   
    auto vec = uintVecToCharVec(spirvCompiled);

    writeCharVecToBinary(vec, "tmp.spv");

    // Load binary file and original file compiled with glslangValidator
    auto compiled = readFile("blur_comp.spv");
    auto read = readFile("tmp.spv");

    // Check that files are equal
    EXPECT_EQ(read, compiled);
    // Check that function returned true
    EXPECT_EQ(true, result);
}

TEST_F(SpvShaderCompilerTests, compileGLSLFromFile_CompileFileWithErrors)
{
    bool result = compiler.compileGLSLFromFile("blur_errors.comp");

    EXPECT_EQ(false, result);
}

TEST_F(SpvShaderCompilerTests, compileGLSLFromCode_CompareToOutputOfGlslangValidator)
{
    const char* code = R"( 
        #version 450
        layout (local_size_x = 16, local_size_y = 16) in;
        layout (binding = 0, rgba8) uniform readonly image2D inputImage;
        layout (binding = 1, rgba8) uniform image2D resultImage;
        
        void main()
        {   
            float weight[5];
        	weight[0] = 0.227027;
        	weight[1] = 0.1945946;
        	weight[2] = 0.1216216;
        	weight[3] = 0.054054;
        	weight[4] = 0.016216;
        
            float blurScale = 1.0;
            float blurStrength = 30.0;
        
            ivec2 inSize = imageSize(inputImage);
        
            float texOffset = blurScale / 5.0;
        
            vec3 result = imageLoad(inputImage, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y)).rgb;
        
            for(int i = 1; i < 5; ++i)
        	{
        		// H
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) + ivec2(blurScale + i, 0.0)).rgb * weight[i] * blurStrength;
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) - ivec2(blurScale - i, 0.0)).rgb * weight[i] * blurStrength;
        		// V
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, blurScale + i)).rgb * weight[i] * blurStrength;
        	    result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) - ivec2(0.0, blurScale - i)).rgb * weight[i] * blurStrength;
        	}
        
            vec4 res = vec4(result / blurStrength, 1.0);
        
            imageStore(resultImage, ivec2(gl_GlobalInvocationID.xy), res);
        } )";

    std::string codeString= std::string(code);

    bool result = compiler.compileGLSLFromCode(codeString, "comp");
    auto spirvCompiled = compiler.getSpirV();

    auto vec = uintVecToCharVec(spirvCompiled);

    writeCharVecToBinary(vec, "tmp2.spv");

    // Load binary file and original file compiled with glslangValidator
    auto compiled = readFile("blur_comp.spv");
    auto read = readFile("tmp2.spv");

    // Check that files are equal
    EXPECT_EQ(read, compiled);
    // Check that function returned true
    EXPECT_EQ(true, result);
}

TEST_F(SpvShaderCompilerTests, compileGLSLFromCode_CompileCodeWithErrors)
{
    const char* code = R"( 
        #version 450
        layout (local_size_x = 16, local_size_y = 16) in;
        layout (binding = 0, rgba8) uniform readonly image2D inputImage;
        //layout (binding = 1, rgba8) uniform image2D resultImage;
        
        void main()
        {   
            float weight[5];
        	weight[0] = 0.227027;
        	weight[1] = 0.1945946;
        	weight[2] = 0.1216216;
        	weight[3] = 0.054054;
        	weight[4] = 0.016216;
        
            float blurScale = 1.0;
            float blurStrength = 30.0;
        
            ivec2 inSize = imageSize(inputImage);
        
            float texOffset = blurScale / 5.0;
        
            vec3 result = imageLoad(inputImage, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y)).rgb;
        
            for(int i = 1; i < 5; ++i)
        	{
        		// H
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) + ivec2(blurScale + i, 0.0)).rgb * weight[i] * blurStrength;
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) - ivec2(blurScale - i, 0.0)).rgb * weight[i] * blurStrength;
        		// V
        		result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, blurScale + i)).rgb * weight[i] * blurStrength;
        	    result += imageLoad(inputImage, ivec2(gl_GlobalInvocationID.xy) - ivec2(0.0, blurScale - i)).rgb * weight[i] * blurStrength;
        	}
        
            vec4 res = vec4(result / blurStrength, 1.0);
        
            imageStore(resultImage, ivec2(gl_GlobalInvocationID.xy), res);
        } )";

    std::string codeString = std::string(code);

    bool result = compiler.compileGLSLFromCode(codeString, "comp");

    EXPECT_EQ(false, result);
}

