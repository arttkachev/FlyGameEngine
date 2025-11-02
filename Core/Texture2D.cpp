#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION // generates implementation of STB LIB. It must be done before "include the header" It specified so by the lib creator
// lib to load images. OpenGL doesn't have this functionality by default
#include "stb_image/stb_image.h"
#include <iostream>

Texture2D::Texture2D()
  : mTexture(0)
{
}

Texture2D::~Texture2D()
{

}

bool Texture2D::loadTexture(const string & filename, bool generateMipMaps)
{
  // will be passed to a method to read image from disk
  int width{}, height{}, components{};

  // stb lib will read the file from disk. Allocate array of bytes and return
  // a pointer to that array
  // c_str() means C string because of a method returns a char pointer
  // args (filename, width, height, components(RGBA), type of image) Last enum can be found in the stb_image.h meaning RGBA image
  // 4 components type of image to read
  unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
  if (imageData == nullptr)
  {
    std::cerr << "Error loading texture '" << filename << "'" << std::endl;
    return false;
  }

  // flip image vertically because it loads in upside down
  // we use a trick with a pointer arithmetic
  // we could also do it in any place like in a shader itself but decided to flip it right on loading image
  // we need width in bytes to reverse bytes in the image from bottom to top by myltiplying this width in bytes
  int widthInBytes = width * 4; // because 1 pixel = 8 bits. 8 * 4 = 32 bits or 4 bytes
  unsigned char* top = nullptr;
  unsigned char* bottom = nullptr;
  unsigned char temp = 0;
  int halfHeight = height / 2;
  // going along a half of the texture vertically
  for (int row = 0; row < halfHeight; row++)
  {
    top = imageData + row * widthInBytes; // going through top 
    bottom = imageData + (height - row - 1) * widthInBytes; // going through bottom
    for (int col = 0; col < widthInBytes; col++)
    {
      // actual reversing
      temp = *top; // store top value in a temp var. Dereference *top because temp is not a ptr
      *top = *bottom; // reverse top and bottom values dereferencing them
      *bottom = temp; // assign former top value to bottom
      top++; // iterate with a loop
      bottom++; // iterate with a loop
    }

  }


  // Generate textures like we generated buffers
  // args (number of texture, texture handler which will store a generated texture
  glGenTextures(1, &mTexture);

  // bind texture args (what kind of texture to bind, handler that holds a texture
  glBindTexture(GL_TEXTURE_2D, mTexture);

  // next we are going to assign parameters of the generated texture
  // wrapping mode, filtering
  // glTexParameteri() method to set these parameters
  // args (type of texture, type of parameter (in this case horizontal(S) wrapping), 
  // selected mode - we are going to use repeat pattern (a texture will be repeated across the rest of the polygon once hit the end of the image) 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  // vertical wrapping mode (T). Also repeat the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // filtering mode if the texture is bigger than the polygon (GL_TEXTURE_MIN_FILTER) - MIN = Minification
  // we minimize the texture. We are going to use Linear Filtering (average the pixel between the pixel nearest to the fragment and surrounding pixels) More smooth
  // GL_NEREST = filter mode that selects a nereast to the fragment pixel color. It's more sharp
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // filtering mode if the texture is smaller than the polygon (GL_TEXTURE_MAG_FILTER) - MAG = Magnification
  // we maximize the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // next we're gonna map bits from the image we loaded to the OpenGL texture (convert a loaded image to the OpenGL texture)
  // args (type of texture, particular level of detail 0 = full resolution of the texture,
  // internal format of the texture, width, height, width of border 0 = no border,
  // resulting texture format, type of data we're passing in to create this OpenGL texture,
  // OpenGL needs to know what size of data we're passing in, data itself (pointer to
  // array of bytes we loaded with stbi loader))
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

  if (generateMipMaps)
  {
    // OpenGL does this work for us. It doesn't modify an original image
    // It modifies internal OpenGL GL_TEXTURE_2D object to generate mipmaps 
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  // we need to clean our memory because we are done
  // once we called glTexImage2D() and passed in imagData array of data OpenGL generated a texture
  // we don't need to allocate memory anymore for unsigned char* imageData
  // free memory (args: pointer to the image data)
  stbi_image_free(imageData);

  // unbind the texture because we're done. We did all work and don't want to affect
  // the texture somehow. We unbind the texture by passing the second 0 argument
  // it stops tracking any action on our texture
  glBindTexture(GL_TEXTURE_2D, 0);
  return true;
}

void Texture2D::bind(GLint texUnit)
{
  // this method for multiple texturing
  // it guarantees to have a single texture bound to up to 16 texture units
  // (number is determined by a specific video card) that allows using blending of multiple textures
  // GL_TEXTURE0 is the first texture unit
  // + texUnit we use different tex units passed by an arg. We handle using multiple tex units by this way
  glActiveTexture(GL_TEXTURE0 + texUnit);

  // args: (type of texture we deal, texture handler)
  // bind to listen to actions on the texture
  glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture2D::unbind(GLint texUnit)
{
  // do the same as in the method above
  glActiveTexture(GL_TEXTURE0 + texUnit);

  // but just pass in 0 as a second argument to unbind
  glBindTexture(GL_TEXTURE_2D, 0);
}
