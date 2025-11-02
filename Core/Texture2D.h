#pragma once

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "GL\glew.h"
#include <string>
using std::string;

class Texture2D
{
public:

  Texture2D();
  virtual ~Texture2D();

  bool loadTexture(const string& filename, bool generateMipMaps = true);

  // binding texture. Texture unit arg is being used by a sampler. It's being used for multiple textures
  // and allow that a single texture is bound to different texture units for blending multiple textures
  // it also allows to specifty a location of the texture that's going to be used by a sampler
  // see in Main how it's being used
  void bind(GLint texUnit = 0);

  void unbind(GLint texUnit = 0);

private:

  // Like many other OpenGL objects OpenGL holds a texture itself
  // but we need to have a handle to it
  GLuint mTexture{};


};






#endif // !TEXTURE2D_H

