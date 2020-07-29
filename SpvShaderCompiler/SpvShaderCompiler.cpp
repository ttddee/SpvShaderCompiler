#include "SpvShaderCompiler.h"

#include <iostream>

SpvCompiler::SpvCompiler()
{
	// Initialize GLSL
	if (!glslang::InitializeProcess())
	{
		throw std::runtime_error("Failed to initialize glslang.");
	}
}

std::string SpvCompiler::getFilePath(const std::string& path)
{
	size_t parts = path.find_last_of("/\\");
	return path.substr(0, parts);
}

std::string SpvCompiler::getFileName(const std::string& path)
{
	size_t parts = path.find_last_of("/\\");
	return path.substr(parts + 1);
}

std::string SpvCompiler::getFileExtension(const std::string& path)
{
	const size_t pos = path.rfind('.');
	if (pos == std::string::npos)
	{
		return "";
	}
	else 
	{
		return path.substr(path.rfind('.') + 1);
	}
}

EShLanguage SpvCompiler::getShaderStage(const std::string& fileExtension)
{
	if (fileExtension == "vert")
	{
		return EShLangVertex;
	}
	else if (fileExtension == "tesc")
	{
		return EShLangTessControl;
	}
	else if (fileExtension == "tese")
	{
		return EShLangTessEvaluation;
	}
	else if (fileExtension == "geom")
	{
		return EShLangGeometry;
	}
	else if (fileExtension == "frag")
	{
		return EShLangFragment;
	}
	else if (fileExtension == "comp")
	{
		return EShLangCompute;
	}
	else
	{
		std::cout << "getShaderStage: " << fileExtension << std::endl;
		throw std::runtime_error("File extension doesn't match any known shader stage.");
	}
}

bool SpvCompiler::compile(const std::string shaderType, const std::string& shaderCode, std::vector<unsigned int>& spirV)
{
	auto shaderStage = getShaderStage(shaderType);

	const char* inputCString = shaderCode.c_str();

	// Create TShader and pass input to it
	glslang::TShader shader(shaderStage);
	shader.setStrings(&inputCString, 1);

	// Set up resources
	int clientInputSemanticsVersion = 100;
	glslang::EShTargetClientVersion vulkanClientVersion = glslang::EShTargetVulkan_1_0;
	glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_0;

	shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, clientInputSemanticsVersion);
	shader.setEnvClient(glslang::EShClientVulkan, vulkanClientVersion);
	shader.setEnvTarget(glslang::EShTargetSpv, targetVersion);

	TBuiltInResource resources;
	resources = defaultTBuiltInResource;
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

	const int defaultVersion = 100;

	// Preprocessing
	DirStackFileIncluder includer;

	//std::string filepath = getFilePath(path);
	//includer.pushExternalLocalDirectory(filepath);

	std::string preprocessedGLSL;

	if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, includer))
	{
		std::cout << "GLSL Preprocessing Failed for." << std::endl;
		std::cout << shader.getInfoLog() << std::endl;
		std::cout << shader.getInfoDebugLog() << std::endl;

		return false;
	}

	const char* preprocessedCStr = preprocessedGLSL.c_str();
	shader.setStrings(&preprocessedCStr, 1);

	// Parse shader
	if (!shader.parse(&resources, 100, false, messages))
	{
		std::cout << "GLSL parsing failed." << std::endl;
		std::cout << shader.getInfoLog() << std::endl;
		std::cout << shader.getInfoDebugLog() << std::endl;

		return false;
	}

	// Pass to program and link
	glslang::TProgram program;
	program.addShader(&shader);

	if (!program.link(messages))
	{
		std::cout << "GLSL Linking Failed." << std::endl;
		std::cout << shader.getInfoLog() << std::endl;
		std::cout << shader.getInfoDebugLog() << std::endl;

		return false;
	}

	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;
	glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirV, &logger, &spvOptions);

	return true;
}

bool SpvCompiler::compileGLSLFromFile(const std::string& path)
{
	// Open file
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::string inputGLSL((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	auto extension = getFileExtension(path);

	spirV.clear();

	if (compile(extension, inputGLSL, spirV))
	{
		return true;
	}

	return false;
}

bool SpvCompiler::compileGLSLFromCode(const std::string& code, const std::string& shaderType)
{
	spirV.clear();

	if (compile(shaderType, code, spirV))
	{
		return true;
	}

	return false;
}

std::vector<unsigned int> SpvCompiler::getSpirV()
{
	return spirV;
}


