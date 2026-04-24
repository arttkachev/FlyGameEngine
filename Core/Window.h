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

      void initWindow();
      inline bool shouldClose() { return glfwWindowShouldClose(window); };
      void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
      VkExtent2D getExtend() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

    private:
      GLFWwindow* window = nullptr;
      const int32_t width = 0;
      const int32_t height = 0;
      const char* windowTitle = nullptr;
    };
  }
}