#include "Window.h"

#include <stdexcept>
#include "assert.h"

Engine::Core::Window::Window(const int32_t w, const int32_t h, const char* title) :
  width(w), height(h), windowTitle(title)
{
  initWindow();
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
}

Engine::Core::Window::~Window()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}



void Engine::Core::Window::frameBufferResizedCallback(GLFWwindow* window, int32_t width, int32_t height)
{
  auto userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  userWindow->frameBufferResized = true;
  userWindow->width = width;
  userWindow->height = height;
}

void Engine::Core::Window::initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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