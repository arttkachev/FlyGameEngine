#include "Window.h"

#include <stdexcept>
#include "assert.h"

Engine::Core::Window::Window(const int32_t w, const int32_t h, const char* title) :
  width(w), height(h), windowTitle(title)
{
  initWindow();
}

Engine::Core::Window::~Window()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Engine::Core::Window::initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window = glfwCreateWindow(width, height, windowTitle, nullptr, nullptr);
  assert(window != nullptr);
}

void Engine::Core::Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
  {
    throw runtime_error("failed to create window surface");
  }
}