#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"

ShaderProgram::ShaderProgram()
  : mProgramHandler{}
{

}

ShaderProgram::~ShaderProgram()
{

}

bool ShaderProgram::loadShaders(const char * vsFileName, const char * fsFileName)
{
  ////////////CREATE_SHADERS////////////////////
  // read shaders
  string vsString = readShaderFile(vsFileName);
  string fsString = readShaderFile(fsFileName);

  // get source ptr (char*) to shaders to pass them in glCreateShader
  const GLchar* vsSourcePtr = vsString.c_str();
  const GLchar* fsSourcePtr = fsString.c_str();  

  // vertex shader id
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  // fragment shader id
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  // assign vertex shader source
  glShaderSource(vertexShader, 1, &vsSourcePtr, nullptr); // args: identifier, number of shaders, source code of shader, len of shader

  // assign fragment shader source
  glShaderSource(fragmentShader, 1, &fsSourcePtr, nullptr); // args: identifier, number of shaders, source code of shader, len of shader

  // compile vertex shader
  glCompileShader(vertexShader);
  checkCompilierErrors(vertexShader, ShaderType::VERTEX);

  // compile fragment shader
  glCompileShader(fragmentShader);
  checkCompilierErrors(fragmentShader, ShaderType::FRAGMENT);

  //// log error if vertex shader failed to compile
  //GLint result{};
  //GLchar infoLog[512];

  //// get compile result
  //glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

  //if (!result)
  //{
  //  // get log info
  //  glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
  //  std::cout << "Error! Vertex Shader failed to compile" << infoLog << std::endl;
  //}

  //// log error if fragment shader failed to compile
  //// get compile result
  //glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

  //if (!result)
  //{
  //  // get log info
  //  glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
  //  std::cout << "Error! Fragment Shader failed to compile" << infoLog << std::endl;
  //}

  // shader program
  mProgramHandler = glCreateProgram();

  // attach our shaders to a created shader program 
  glAttachShader(mProgramHandler, vertexShader); // args: shader program, attaching shader
  glAttachShader(mProgramHandler, fragmentShader); // args: shader program, attaching shader

  // Link shader program
  glLinkProgram(mProgramHandler);

  // log shader program  
  checkCompilierErrors(mProgramHandler, ShaderType::PROGRAM);

  //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);

  //if (!result)
  //{
  //  // get info log
  //  glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
  //  std::cout << "Error! Shader Program Linker failure!" << infoLog << std::endl;
  //}

  // because we linked shaders to a shader program we don't need them anymore
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // clear Uniform locations. Not neccesary because it's called once in our program
  mUniformLocations.clear();

  ////////////END_CREATE_SHADERS////////////////////
  return true;
}

void ShaderProgram::useProgram()
{
  if (mProgramHandler > 0)
  {
    glUseProgram(mProgramHandler);
  }
}

GLuint ShaderProgram::getShaderProgram() const
{
  // return a handle to our program
  return mProgramHandler;
}

void ShaderProgram::setUniform(const GLchar * name, const glm::vec2 & vec)
{
  // Get Uniform Location from out map
  GLint loc = getUnifomLocation(name);

  // Set Uniform Location. This method is C-like. We cannot have one overloaded 
  // function C++-like
  glUniform2f(loc, vec.x, vec.y);
}

void ShaderProgram::setUniform(const GLchar * name, const glm::vec3 & vec)
{
  // Get Uniform Location from out map
  GLint loc = getUnifomLocation(name);

  // Set Uniform Location
  glUniform3f(loc, vec.x, vec.y, vec.z);
}

void ShaderProgram::setUniform(const GLchar * name, const glm::vec4 & vec)
{
  // Get Uniform Location from our map
  GLint loc = getUnifomLocation(name);

  // Set Uniform Location
  glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void ShaderProgram::setUniform(const GLchar * name, const glm::mat4 & m)
{
  // Get Uniform Location from our map
  GLint loc = getUnifomLocation(name);

  // Set Uniform Location
  // args:(location, number of matrices, transpose or not matrices, ptr to the matrix
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

string ShaderProgram::readShaderFile(const string & fileName)
{
  std::stringstream shaderData{};
  std::ifstream shaderFile{};

  try
  {
    shaderFile.open(fileName, std::ios::in);

    if (!shaderFile.fail())
    {
      // if a file is open, take a ptr to a read buffer and pass it to a string stream
      // now a sting stream has a string (shader loaded from a file)
      shaderData << shaderFile.rdbuf();
    }

    shaderFile.close(); // close a file handle

  }
  catch (const std::exception&)
  {
    std::cerr << "Error reading shader filename!" << std::endl;
  }

  // return string stream with read buffer from a file
  return shaderData.str();
}

void ShaderProgram::checkCompilierErrors(GLint shader, ShaderType type)
{
  GLint status{};

  if (type == ShaderType::PROGRAM)
  {
    // check link status
    glGetProgramiv(mProgramHandler, GL_LINK_STATUS, &status);

    // if the status is not good
    if (status == GL_FALSE)
    {
      // get get log info
      GLint length{};
      glGetProgramiv(mProgramHandler, GL_INFO_LOG_LENGTH, &length);

      string errorLog(length, ' ');
      
      // args: program or shader, log length, ptr to log length, where to put log message
      glGetProgramInfoLog(mProgramHandler, length, &length, &errorLog[0]);
      std::cerr << "Program failed to link! " << errorLog << std::endl;    

    }
  }
  else // VERTEX OR FRAGMENT
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
      // get get log info
      GLint length{};
      glGetShaderiv(shader, GL_COMPILE_STATUS, &length);

      string errorLog(length, ' ');

      // get log info
      // args: program or shader, log length, ptr to log length, where to put log message
      glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
      std::cerr << "Shader failed to compile!" << errorLog << std::endl;
    }
  }
}

GLint ShaderProgram::getUnifomLocation(const GLchar * name)
{
  std::map <string, GLint>::iterator it = mUniformLocations.find(name);

  // if Location is not found we try to find it in the current Shader Program
  if (it == mUniformLocations.end())
  {
    mUniformLocations[name] = glGetUniformLocation(mProgramHandler, name);
  }

  // return found
  return mUniformLocations[name];
}
