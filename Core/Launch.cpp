#include "Launch.h"
#include "Window.h"

#include <stdexcept>
#include <array>

Engine::Core::Launch::Launch()
{
  loadModelData();
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

Engine::Core::Launch::~Launch()
{
  vkDestroyPipelineLayout(renderDevice.device(), pipelineLayout, nullptr);
}

void Engine::Core::Launch::run()
{
  while (gameWindow.shouldClose() != true)
  {
    glfwPollEvents();
    drawFrame();
  }
  // cpu waits until all gpu operations have completed
  vkDeviceWaitIdle(renderDevice.device());
}

void Engine::Core::Launch::loadModelData()
{
  vector<Engine::Render::ModelData::Vertex> vertices
  { 
    {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f }},
    {{0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f }},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f }} 
  };
  modelData = make_unique<Engine::Render::ModelData>(renderDevice, vertices);
}

void Engine::Core::Launch::createPipelineLayout()
{
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;
  
  if (vkCreatePipelineLayout(renderDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    throw runtime_error("failed to create pipeline layout");
  }
}

void Engine::Core::Launch::createPipeline()
{
  auto pipelineConfig = Engine::Render::Pipeline::initDefaultPipelineConfigInfo(swapChain.width(), swapChain.height());
  pipelineConfig.renderPass = swapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  graphicsPipeline = make_unique<Engine::Render::Pipeline>
    (renderDevice, "Shaders/Test_Shader.vert.spv", "Shaders/Test_Shader.frag.spv", pipelineConfig);
}

void Engine::Core::Launch::createCommandBuffers()
{
  commandBuffers.resize(swapChain.imageCount());
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = renderDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(renderDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
  {
    throw runtime_error("failed to allocate command buffer");
  }

  // record command buffer to frame buffers
  for (int i = 0; i < commandBuffers.size(); ++i)
  {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
    {
      throw runtime_error("failed to begin recording command buffers");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain.getRenderPass();
    renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

    array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    // send the first command "Render Pass" to the command buffer
    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    graphicsPipeline->bind(commandBuffers[i]);
    // actual draw call
    //vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
    modelData->bindVertexBuffer(commandBuffers[i]);
    modelData->drawVertexBuffer(commandBuffers[i]);
    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
    {
      throw runtime_error("failed to record to command buffer");
    }
  }
}

void Engine::Core::Launch::drawFrame()
{
  uint32_t imageIndex = 0;
  // fetch which frame it should render next and automatically handles sync between CPU/GPU
  auto result = swapChain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    throw runtime_error("failed to acquire swap chain image");
  }

  // submit command buffer to device
  result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result != VK_SUCCESS)
  {
    throw runtime_error("failed to present swap chain image");
  }

}
