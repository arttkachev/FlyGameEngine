#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"

// a custom data type that's gonna hold vertex data
struct Vertex
{
  glm::vec3 position{};
  glm::vec2 texCoords{};
};

class Mesh
{
public:
  Mesh();
  ~Mesh();

public:

  // method to load OBJ files
  bool loadOBJ(const std::string& filename);

  // draw vertices
  void draw();

private:

  // create buffers VBO and VAO to send vertices to a video card and draw them 
  void initBuffers();

  // flag for internal use to check if we successfully read OBJ before creating buffers
  bool mLoaded{};
  
  // container that holds vertices of a mesh
  std::vector<Vertex> mVertices{};

  // our VAO and VBO that contain vertices of mesh to draw them on the video card
  GLuint mVBO{}, mVAO{}; 
};


#endif // !MESH_H
