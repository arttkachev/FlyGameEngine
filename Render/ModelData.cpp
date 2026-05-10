#include "ModelData.h"

#include <cassert>

namespace Engine
{
  namespace Render
  {
    ModelData::ModelData(RenderDevice& renderDevice, const vector<Vertex>& vertices)
      : renderDevice(renderDevice)
    {
      createVertexBuffers(vertices);
    }

    ModelData::ModelData::~ModelData()
    {
      vkDestroyBuffer(renderDevice.device(), vertextBuffer, nullptr);
      vkFreeMemory(renderDevice.device(), vertexBufferMemory, nullptr);
    }
    void ModelData::drawVertexBuffer(VkCommandBuffer commandBuffer)
    {
      vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
    void ModelData::bindVertexBuffer(VkCommandBuffer commandBuffer)
    {
      VkBuffer buffers[] = { vertextBuffer };
      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }
    void ModelData::createVertexBuffers(const vector<Vertex>& vertices)
    {
      vertexCount = static_cast<uint32_t>(vertices.size());
      assert(vertexCount >= 3 && "vertex count must be at least 3");
      VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
      renderDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertextBuffer, vertexBufferMemory);

      // transfer vertices from cpu to gpu
      void* data;
      vkMapMemory(renderDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
      memcpy(data, vertices.data(), static_cast<uint32_t>(bufferSize));
      vkUnmapMemory(renderDevice.device(), vertexBufferMemory);

    }
    vector<VkVertexInputBindingDescription> ModelData::Vertex::getBindingDescription()
    {
      vector<VkVertexInputBindingDescription> bindingDescription(1);
      bindingDescription[0].binding = 0;
      bindingDescription[0].stride = sizeof(Vertex);
      bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return bindingDescription;
    }

    vector<VkVertexInputAttributeDescription> ModelData::Vertex::getAttributeDescription()
    {
      vector<VkVertexInputAttributeDescription> attributeDescription(2);
      attributeDescription[0].binding = 0;
      attributeDescription[0].location = 0; // corresponds to location in shader
      attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescription[0].offset = offsetof(Vertex, position);

      attributeDescription[1].binding = 0;
      attributeDescription[1].location = 1; // corresponds to location in shader
      attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescription[1].offset = offsetof(Vertex, color);
      return attributeDescription;
    }
  }
}