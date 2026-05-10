#pragma once

#include "Render/RenderDevice.h"

#include <string>
#include <vector>

using namespace std;

namespace Engine
{
  namespace Render
  {
    struct PipelineConfigInfo
    {
      PipelineConfigInfo(const PipelineConfigInfo&) = delete;
      PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

      // fixed render stage options
      VkPipelineViewportStateCreateInfo viewportInfo;
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
      VkPipelineRasterizationStateCreateInfo rasterizationInfo;
      VkPipelineMultisampleStateCreateInfo multisampleInfo;
      VkPipelineColorBlendAttachmentState colorBlendAttachment;
      VkPipelineColorBlendStateCreateInfo colorBlendInfo;
      VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
      vector<VkDynamicState> dynamicStateEnables;
      VkPipelineDynamicStateCreateInfo dynamicStateInfo;
      VkPipelineLayout pipelineLayout = nullptr;
      VkRenderPass renderPass = nullptr;
      uint32_t subpass = 0;
    };
    class Pipeline
    {
    public:
      Pipeline(Engine::Render::RenderDevice& device, const string& vertFilepath, const string& fragFilepath, const PipelineConfigInfo& configInfo);
      ~Pipeline();
      Pipeline(const Pipeline&) = delete;
      Pipeline& operator=(const Pipeline&) = delete;

      void bind(VkCommandBuffer commandBuffer);
      static void initDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
      static vector<char> readFile(const string& filepath);
      void createGraphicsPipeline(const string& vertFilepath, const string& fragFilepath, const PipelineConfigInfo& configInfo);
      void createShaderModule(vector<char>& shaderCode, VkShaderModule* shaderModule);

      // handlers
      // implicit relationship.Pipeline cannot exists without device
      // aka "aggregation", but potentially memory unsafe
      // if we release device before pipeline object and could access to a device member
      Engine::Render::RenderDevice& renderDevice;
      VkPipeline graphicsPipeline = nullptr;
      VkShaderModule vertShaderModule = nullptr;
      VkShaderModule fragShaderModule = nullptr;
    };
  }

}