#pragma once

#include <string>
#include <vector>
#include <memory>

class SpvCompiler {
public:
    SpvCompiler();
    ~SpvCompiler();

	bool compileGLSLFromFile(const std::string& path);
    bool compileGLSLFromCode(const std::string& code, const std::string& shaderType);

    std::vector<unsigned int> getSpirV();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

	/*std::string getFilePath(const std::string& s);
	std::string getFileName(const std::string& s);
	std::string getFileExtension(const std::string& s);*/

    //std::vector<unsigned int> spirV;

	/*EShLanguage getShaderStage(const std::string& fileExtension);

    bool compile(const std::string& shaderType, const std::string& shaderCode, std::vector<unsigned int>& spirV);*/

    //static const TBuiltInResource defaultTBuiltInResource;
};

