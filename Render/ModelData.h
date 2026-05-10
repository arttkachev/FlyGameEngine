#pragma once

#include "Render/RenderDevice.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <vector>

namespace Engine
{
  namespace Render
  {
    using namespace std;
    class ModelData
    {
    public:

      struct Vertex
      {
        glm::vec2 position;
        glm::vec3 color;
        static vector<VkVertexInputBindingDescription> getBindingDescription();
        static vector<VkVertexInputAttributeDescription> getAttributeDescription();


      };

      ModelData(RenderDevice& renderDevice, const vector<Vertex>& vertices);
      ~ModelData();

      ModelData(const ModelData&) = delete;
      ModelData& operator=(const ModelData&) = delete;

      void bindVertexBuffer(VkCommandBuffer commandBuffer);
      void drawVertexBuffer(VkCommandBuffer commandBuffer);

    private:
      void createVertexBuffers(const vector<Vertex>& vertices);
      RenderDevice& renderDevice;
      VkBuffer vertextBuffer;
      VkDeviceMemory vertexBufferMemory;
      uint32_t vertexCount = 0;


    };
  }
}