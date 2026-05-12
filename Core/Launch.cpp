#include "Launch.h"
#include "Window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <stdexcept>
#include <array>

// temp
namespace Engine
{
  namespace Render
  {
    struct SimplePushConstantData
    {
      glm::vec2 offset;
      alignas(16) glm::vec3 color;
    };
  }
}

Engine::Core::Launch::Launch()
{
  loadModelData();
  createPipelineLayout();
  //createPipeline();
  recreateSwapChain();
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
  // push constant range setup
  VkPushConstantRange pushConstantRange{};
  // push constant data is available in vertext and fragment shader
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = 32;//sizeof(Engine::Render::SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(renderDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    throw runtime_error("failed to create pipeline layout");
  }
}

void Engine::Core::Launch::createPipeline()
{
  assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline loyout");

  Engine::Render::PipelineConfigInfo pipelineConfigInfo{};
  Engine::Render::Pipeline::initDefaultPipelineConfigInfo(pipelineConfigInfo);
  pipelineConfigInfo.renderPass = swapChain->getRenderPass();
  pipelineConfigInfo.pipelineLayout = pipelineLayout;
  graphicsPipeline = make_unique<Engine::Render::Pipeline>
    (renderDevice, "Shaders/Test_Shader.vert.spv", "Shaders/Test_Shader.frag.spv", pipelineConfigInfo);
}

void Engine::Core::Launch::createCommandBuffers()
{
  commandBuffers.resize(Engine::Render::SwapChain::MAX_FRAMES_IN_FLIGHT);
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = renderDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(renderDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
  {
    throw runtime_error("failed to allocate command buffer");
  }
}

void Engine::Core::Launch::freeCommandBuffers()
{
  vkFreeCommandBuffers(renderDevice.device(), renderDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
  commandBuffers.clear();
}

void Engine::Core::Launch::recordCommandBuffer(int32_t imageIndex)
{
  static int32_t frame = 0;
  frame = (frame + 1) % 1000;
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
  {
    throw runtime_error("failed to begin recording command buffers");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain->getRenderPass();
  renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

  // clear values = background
  array<VkClearValue, 2> clearValues{};
  clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  // send the first command "Render Pass" to the command buffer
  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  graphicsPipeline->bind(commandBuffers[imageIndex]);
  // actual draw call
  //vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
  modelData->bindVertexBuffer(commandBuffers[imageIndex]);

  for (int32_t j = 0; j < 4; j++)
  {
    Engine::Render::SimplePushConstantData push{};
    push.offset = { -0.5f + frame * 0.002f, -0.4f + j * 0.25f };
    push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };
    vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
      sizeof(Engine::Render::SimplePushConstantData), &push);
    // draw each copy for shaders with slightly different data
    modelData->drawVertexBuffer(commandBuffers[imageIndex]);
  }

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
  {
    throw runtime_error("failed to record to command buffer");
  }
}

void Engine::Core::Launch::recreateSwapChain()
{
  auto extent = gameWindow.getExtend();
  while (extent.width == 0 || extent.height == 0)
  {
    extent = gameWindow.getExtend();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(renderDevice.device());

  if (swapChain == nullptr)
  {
    swapChain = nullptr;
    swapChain = make_unique<Engine::Render::SwapChain>(renderDevice, extent);
  }
  else
  {
    swapChain = make_unique<Engine::Render::SwapChain>(renderDevice, extent, move(swapChain));
    if (swapChain->imageCount() != commandBuffers.size())
    {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }
  // TODO: check if render pass compatible do nothing else
  createPipeline();
}

void Engine::Core::Launch::drawFrame()
{
  uint32_t imageIndex = 0;
  // fetch which frame it should render next and automatically handles sync between CPU/GPU
  auto result = swapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    throw runtime_error("failed to acquire swap chain image");
  }

  recordCommandBuffer(imageIndex);
  // submit command buffer to device
  result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || gameWindow.wasFrameBufferResized())
  {
    gameWindow.ResetFrameBufferResizedFlag();
    recreateSwapChain();
    return;
  }
  if (result != VK_SUCCESS)
  {
    throw runtime_error("failed to present swap chain image");
  }
}