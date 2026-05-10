#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Engine
{
  namespace Core
  {
    using namespace std;
    class Window
    {
    public:
      Window(const int32_t w, const int32_t h, const char* title);
      ~Window();
      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      inline bool shouldClose() { return glfwWindowShouldClose(window); };
      void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
      VkExtent2D getExtend() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
      inline bool wasFrameBufferResized() { return frameBufferResized; }
      void ResetFrameBufferResizedFlag() { frameBufferResized = false; }

    private:
      static void frameBufferResizedCallback(GLFWwindow* window, int32_t width, int32_t height);
      void initWindow();
      GLFWwindow* window = nullptr;
      int32_t width = 0;
      int32_t height = 0;
      bool frameBufferResized = false;
      const char* windowTitle = nullptr;
    };
  }
}