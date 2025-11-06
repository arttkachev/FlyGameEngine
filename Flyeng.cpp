#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Core/Types/CoreTypes.h"
#include <glm/mat4x4.hpp>
#include <iostream>
#include <sstream>

namespace Engine
{
  bool IsAppRunning = false;
  namespace Window
  {
    GLFWwindow* AppWindow = nullptr;
    const char* AppTitle = "Flyeng v1.0";
    bool IsFullScreen = true;
    uint16 AppWindowWidth = 1440;
    uint16 AppWindowHeight = 1080;
  }
}

static bool CreateWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  if (Engine::Window::IsFullScreen == true)
  {
    GLFWmonitor* CurrentMonitor = glfwGetPrimaryMonitor();
    if (CurrentMonitor == nullptr)
    {
      return false;
    }
    const GLFWvidmode* ViewMode = glfwGetVideoMode(CurrentMonitor);
    if (ViewMode == nullptr)
    {
      return false;
    }
    Engine::Window::AppWindow = glfwCreateWindow(ViewMode->width, ViewMode->height, Engine::Window::AppTitle, nullptr, nullptr);
  }
  else
  {
    Engine::Window::AppWindow = glfwCreateWindow(Engine::Window::AppWindowWidth, Engine::Window::AppWindowHeight, Engine::Window::AppTitle, nullptr, nullptr);
  }

  if (Engine::Window::AppWindow == nullptr)
  {
    return false;
  }

  // TODO Transfer code to Vulkan Init
  uint32 ExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
  if (ExtensionCount <= 0)
  {
    return false;
  }
  std::cout << "Number of Vulkan extensions " << ExtensionCount << std::endl;

  return true;
}

static bool DestroyWindow()
{
  if (Engine::Window::AppWindow == nullptr)
  {
    return false;
  }
  glfwDestroyWindow(Engine::Window::AppWindow);
  return true;
}

static bool TerminateEngine()
{
  std::cout << "Engine should close" << std::endl;
  DestroyWindow();
  glfwTerminate();
  return true;
}

static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    Engine::IsAppRunning = false;
  }
}

static bool InitEngine()
{
  if (CreateWindow() == false)
  {
    std::cerr << "Failed to create window";
    TerminateEngine();
    return false;
  }
  std::cout << "Engine should start" << std::endl;
  Engine::IsAppRunning = true;
  glfwSetKeyCallback(Engine::Window::AppWindow, OnKeyPressed);
  return true;
}

static bool RunMainLoop()
{
  glfwPollEvents();
  // update game logic
  // update renderer
  return true;
}

static void RunEngine()
{
  while (Engine::IsAppRunning == true)
  {
    RunMainLoop();
  }
  TerminateEngine();
}


int main()
{
  InitEngine();
  RunEngine();
  return 0;
}