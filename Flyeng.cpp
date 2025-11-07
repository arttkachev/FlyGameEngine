#pragma once

#include "Windows.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Core/Types/CoreTypes.h"
#include <glm/mat4x4.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>

namespace Engine
{
  bool LoadVulkan()
  {
#if defined (_WIN32) || (_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
    // dynamically load vulkan lib
    HMODULE vkLib = LoadLibraryA("vulkan-1.dll"); // HMODULE holds a base address of dll
    if (vkLib == nullptr)
    {
      std::cerr << "vulkan-1.dll is failed to load" << std::endl;
      return false;
    }

    // GetProcAddress returns the address of a function inside dll ("vkCreateInstance")
    PFN_vkCreateInstance pfnVkCreateInstance = (PFN_vkCreateInstance)GetProcAddress(vkLib, "vkCreateInstance");
    if (pfnVkCreateInstance == nullptr)
    {
      std::cerr << "Failed to retrieve the address of vkCreateInstance" << std::endl;
      return false;
    }
    std::cout << "vulkan-1.dll is successfully loaded" << std::endl;
#endif
    return true;
  }

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

static bool CreateVulkan()
{
  return Engine::LoadVulkan();
}

static bool DestroyVulkan()
{
  return true;
}

static bool CreateGLFWWindow()
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

static bool DestroyGLFWWindow()
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
  DestroyGLFWWindow();
  glfwTerminate();
  DestroyVulkan();
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
  if (CreateGLFWWindow() == false)
  {
    std::cerr << "Failed to create window" << std::endl;
    TerminateEngine();
    return false;
  }
  std::cout << "Engine should start" << std::endl;
  Engine::IsAppRunning = true;
  glfwSetKeyCallback(Engine::Window::AppWindow, OnKeyPressed);

  if (CreateVulkan() == false)
  {
    std::cerr << "Failed to create renderer" << std::endl;
    TerminateEngine();
    return false;
  }
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