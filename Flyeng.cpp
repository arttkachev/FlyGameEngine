#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h" // a lib to create window, open, manage input and create contexts
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <iostream>
#include <sstream>

using int32 = int32_t;
using uint32 = uint32_t;
using int16 = int16_t;
using uint16 = uint16_t;


int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* Window = glfwCreateWindow(800, 600, "Flyeng v1.0", nullptr, nullptr);
  if (Window == nullptr)
  {
    std::cerr << "Failed to create window";
    return -1;
  }

  uint32 ExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
  std::cout << "Number of Vulkan extensions " << ExtensionCount << std::endl;

  while (!glfwWindowShouldClose(Window))
  {
    glfwPollEvents();
  }
  glfwDestroyWindow(Window);
  return 0;
}