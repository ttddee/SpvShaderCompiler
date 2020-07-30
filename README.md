# SpvShaderCompiler
**Online compiler from GLSL shader code to SPIR-V**

Based on: https://forestsharp.com/glslang-cpp/

This compiles GLSL shaders into SPIR-V for use in Vulkan. 

CMake is configured to create a static library. To compile the project you need [glslang](https://github.com/KhronosGroup/glslang). If you also want to run the unit tests you need [googletest](https://github.com/google/googletest).
<br/>

# Usage

Include the header:

```
#include SpvShaderCompiler.h
```
<br/>

Create a SpvCompiler object:

```
auto compiler = SpvCompiler();
```
<br/>

There are two public member functions:

```
bool compileGLSLFromFile(const std::string& path);
```
Expects a path to a file containing GLSL shader code and compiles it to SPIR-V.

The shader type is determined by the file extension. For valid extensions see below.
<br/>
<br/>

```
bool compileGLSLFromCode(const std::string& code, const std::string& shaderType);
```
Expects GLSL shader code in a string and a shader type string.
<br/>

Valid shader types are:
- **"vert"** for Vertex Shader
- **"tesc"** for Tesselation Control Shader
- **"tese"** for Tesselation Evaluation Shader
- **"geom"** for Geometry Shader
- **"frag"** for Fragment Shader
- **"comp"** for Compute Shader
<br/>

Both functions return true on success and false on error.
<br/>

To get the compiled SPIR-V code as a vector of unsigned ints:

```
compiler.getSpirV();
```

