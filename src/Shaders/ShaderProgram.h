#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "GL/glew.h"
#include <stdint.h>
#include <string>
#include <map>
#include "glm\glm.hpp"

using std::string;

class ShaderProgram
{
public:
  ShaderProgram();
  ~ShaderProgram();

  enum class ShaderType : uint8_t
  {
    VERTEX,
    FRAGMENT,
    PROGRAM,
  };

  bool loadShaders(const char* vsFileName, const char* fsFileName);

  // ativate a shader program
  void useProgram();

  // Uniforms are global vars "In" visible by all shaders
  void setUniform(const GLchar* name, const glm::vec2& vec);
  void setUniform(const GLchar* name, const glm::vec3& vec);
  void setUniform(const GLchar* name, const glm::vec4& vec);

private:

  // load shader string from a file
  string readShaderFile(const string& fileName);
  void checkCompilierErrors(GLint shader, ShaderType type);

  // Utility method to get Uniform Location. We don't want to call it every time in Game Loop
  GLint getUnifomLocation(const GLchar* name);

  // Program Handler
  GLint mProgramHandler{};

  // Map to hold uniforms
  std::map<string, GLint> mUniformLocations;

};

#endif