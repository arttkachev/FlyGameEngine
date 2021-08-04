#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>


Mesh::Mesh()
  : mLoaded{ false }
{

}


Mesh::~Mesh()
{
  // clean up on dtr

  // delete vertex array object
  // args (number of arrays, address of the array)
  glDeleteVertexArrays(1, &mVAO);

  // delete position vertex buffer object (separate buffer layout)
  // args (number of buffers, address of the buffer)
  glDeleteBuffers(1, &mVBO);
}

bool Mesh::loadOBJ(const std::string & filename)
{
  // temp containers to hold data while reading obj files
  std::vector<unsigned int> vertexIndices, uvIndices;
  std::vector<glm::vec3> tempVertices;
  std::vector<glm::vec2> tempUVs;

  // check if the file has obj extention
  if (filename.find(".obj") != std::string::npos)
  {
    std::ifstream fin(filename, std::ios::in); // open a file with input file stream
    if (!fin)
    {
      // failed to open
      std::cerr << "Cannot open " << filename << std::endl;
      return false;
    }

    // if the file found we display a message
    std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

    std::string lineBuffer;

    // reading obj file
    while (std::getline(fin, lineBuffer)) // getting a line by line from the file if found
    {
      if (lineBuffer.substr(0, 2) == "v ") // looking for a "v " (vertex data) in the file (0, 2) - look for 2 characters from the begining of the each line
      {
        std::istringstream v(lineBuffer.substr(2)); // input string stream reads the line after substr 2
        glm::vec3 vertex;
        v >> vertex.x; v >> vertex.y; v >> vertex.z; // write data from the file after substring 2 into vec3 in sequence
        tempVertices.push_back(vertex); // add vertex data into temp container
      }
      else if (lineBuffer.substr(0, 2) == "vt") // looking for a "vt" (vertex texture data) in the file (0, 2) - look for 2 characters from the begining of the each line
      {
        std::istringstream vt(lineBuffer.substr(3)); // the same algorithm goes here as above. Write uv data into vec2 and add into a temp container
        glm::vec2 uv; // ignore w
        vt >> uv.s; vt >> uv.t; // look at obj file to understand this process how it reads and writes
        tempUVs.push_back(uv);
      }
      else if (lineBuffer.substr(0, 2) == "f ") // looking for a "f " (faces data) in the file (0, 2) - look for 2 characters from the begining of the each line
      {
        int p1, p2, p3; //to store mesh index
        int t1, t2, t3; //to store texture index
        int n1, n2, n3;
        const char* face = lineBuffer.c_str();
        int match = sscanf_s(face, "f %i/%i/%i %i/%i/%i %i/%i/%i", // read triangles by 3 point and writes them into vars below
          &p1, &t1, &n1,
          &p2, &t2, &n2,
          &p3, &t3, &n3);
        if (match != 9) // sscanf has to return int with 9 elements (triangle has 3 points) each point = position (p1), texture coords (t1), normal(n1). See vars above
          std::cout << "Failed to parse OBJ file using our very simple OBJ loader" << std::endl;

        // We are ignoring normals (for now)
        // fill indices list from gotten data above
        vertexIndices.push_back(p1);
        vertexIndices.push_back(p2);
        vertexIndices.push_back(p3);
        // fill UV indices list from gotten data above
        uvIndices.push_back(t1);
        uvIndices.push_back(t2);
        uvIndices.push_back(t3);
      }
    }

    // Close the file
    fin.close();


    // For each vertex of each triangle
    // process data from temp containers and create data that VBO is going to use
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
      // Get the attributes using the indices
      // intermediate step before creating final data
      // back to vertex and uv data
      glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
      glm::vec2 uv = tempUVs[uvIndices[i] - 1];

      // assign final values
      Vertex meshVertex;
      meshVertex.position = vertex;
      meshVertex.texCoords = uv;

      mVertices.push_back(meshVertex);
    }

    // Create and initialize the buffers
    initBuffers();

    return (mLoaded = true);
  }

  // We shouldn't get here so return failure
  return false;
}

void Mesh::draw()
{
  if (!mLoaded) return;

  // we do this every time we draw our array
  glBindVertexArray(mVAO);

  // draw array
  // args (type of what we draw, first element (vertex in the array), last element (vertex) in the array)
  glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

  // unbinding. Close it down by passing 0 as an argument
  // by this (0 arg) we tell OpenGL we're done with our vertex array object and other code won't have an access to it
  // OpenGL closes this vertex array object not to allow errors through code (inadvertent remove or something like this)
  glBindVertexArray(0);
}

void Mesh::initBuffers()
{
  // NOW WE'RE GOING TO GENERATE BUFFER AND ARRAY HERE, NOT IN THE MAIN.CPP
  // WE JUST COPIED CODE FROM THERE

  // generate actual vertext buffer object
  // it creates a chunk of memory in the graphics card for us
  glGenBuffers(1, &mVBO); //args: number of buffers, it returns back an identifer for the buffer through the variable vbo

  // makes a created buffer as a current buffer. Only one buffer at a time can be active in OpenGL
  glBindBuffer(GL_ARRAY_BUFFER, mVBO); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  // fill our buffer with data
  // after these 3 calls above we created a buffer in GPU and copied our triangle data (vertices) to it
  // mVertices.size() * sizeof(Vertex) we get the size of the buffer in bytes we need mVertices.size() = number of elements in the container
  // sizeof(Vertex) = size of one element in the container
  // &mVertices[0] = ptr to the first element. Arg = address of data
  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // args: kind of buffer, its size, actural data, type of drawing (STATIC/DYNAMIC/STREAM)

  //// generate actual vertext buffer object
  //// it creates a chunk of memory in the graphics card for us
  //glGenBuffers(1, &vbo_color); //args: number of buffers, it returns back an identifer for the buffer through the variable vbo

  //// makes a created buffer as a current buffer. Only one buffer at a time can be active in OpenGL
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_color); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  //// fill our buffer with data
  //// after these 3 calls above we created a buffer in GPU and copied our triangle data (vertices) to it
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW); // args: kind of buffer, its size, actural data, type of drawing (STATIC/DYNAMIC/STREAM)

  ////////////// VERTEX ARRAY OBJECT//////////////
  // next we need to have vertext array object to draw that holds a vertex buffer object
  // its identifier

  // Gen vertex array object in the same fashion we generated a buffer above
  glGenVertexArrays(1, &mVAO); // number, bind identifier

  // make it an active vertex array object by binding it
  glBindVertexArray(mVAO);

  //// call it agian because only 1 buufer might be active at a given time. We ensure we work with an appropriate buffer next call
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_position); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  // we need to tell a vertext shader how to interpret a buffer (give it a format of vertices in memory (layout in memory))  
  // buffer is a just bytes
  // we do this with this call
  // IMPORTANT: before this call we need to have vao object bound

  //POSITION
  // layout for position. Add position to our vertext buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr); // args: attribute index (0 - position attribute index. it's specified by OpenGL)
  // with UV data presented in vetex buffer it turns to 5 * sizeof(GLfloat) because 5 XYZUV elements in between (to next position data),
  // number of elements in data (3 - specifies each vertex position in our triangle data), type of data, does it need to be normalized, 
  // stride (space between elements) = sizeof(GLfloat) * 6 because 6 elements before next position element in the interleaved layout, 
  // stride/offset (space before the first element) = nullptrt because it starts from 0 byte 

  // by default VertexAttrib is disabled in OpenGL. We need to enable it
  glEnableVertexAttribArray(0); //args: attrib index (0 = position) in our vertex attribute array


  // TEX COORDS
  // add UV coordinates to our vertext buffer
  // 1 means the second elemnt (UV). 0 was for position. 2 means 2 elements in this element U and V floats
  // 5 * sizeof(GLfloat) because 5 elemtns XYZUV
  //(GLvoid*)(3 * sizeof(GLfloat)) means void pointer to 3 (3 * sizeof(GLfloat)) because 3 elements XYZ before first UV elemtns
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  // as before enable our array vertex attribute for UV data (1)
  glEnableVertexAttribArray(1);

  // by this (0 arg) we tell OpenGL we're done with our vertex array object and other code won't have an access to it
  // OpenGL closes this vertex array object not to allow errors through code (inadvertent remove or something like this)
  glBindVertexArray(0);
}
