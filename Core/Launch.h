#pragma once

#include "Core/Window.h"
#include "Render/Pipeline.h"
#include "Render/RenderDevice.h"
#include "Render/SwapChain.h"
#include "Render/ModelData.h"

#include <memory>
#include <vector>

using namespace std;

namespace Engine
{
  namespace Core
  {
    class Launch
    {
    public:
      Launch();
      ~Launch();

      Launch(const Launch&) = delete;
      Launch& operator=(const Launch&) = delete;

      static constexpr int32_t WIDTH = 1024;
      static constexpr int32_t HEIGHT = 768;
      const char* TITLE = "Flyeng v.0.1";
      void run();

    private:
      void loadModelData();
      void createPipelineLayout();
      void createPipeline();
      void createCommandBuffers();
      void freeCommandBuffers();
      void drawFrame();
      void recreateSwapChain();
      void recordCommandBuffer(int32_t imageIndex);

      Engine::Core::Window gameWindow{ WIDTH, HEIGHT, TITLE };
      Engine::Render::RenderDevice renderDevice{ gameWindow };
      unique_ptr<Engine::Render::SwapChain> swapChain{};
      unique_ptr<Engine::Render::Pipeline> graphicsPipeline{};
      VkPipelineLayout pipelineLayout{};
      vector<VkCommandBuffer> commandBuffers{};
      unique_ptr<Engine::Render::ModelData> modelData{};
    };
  }
}