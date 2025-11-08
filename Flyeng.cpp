#pragma once

#include "Windows.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Core/Types/CoreTypes.h"
#include <glm/mat4x4.hpp>
#include <iostream>
#include <sstream>
#include <string>

#define ENGINE_ERROR(error) \
    std::cerr << ("[error][%s:%u] %s", __func__, __LINE__, error) << std::endl;

template <typename... Args>
constexpr void ENGINE_LOG(Args&& ... args)
{
  ((std::cout << " " << std::forward<Args>(args)), ...) << std::endl;
}

namespace Engine
{
  bool IsAppRunning = false;
  static HMODULE vkLib = nullptr;
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
#if defined (_WIN32) || (_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
  // dynamically load vulkan lib
  Engine::vkLib = LoadLibraryA("vulkan-1.dll"); // HMODULE holds a base address of dll
  if (Engine::vkLib == nullptr)
  {
    ENGINE_ERROR("vulkan-1.dll is failed to load");
    return EXIT_FAILURE;
  }

  // GetProcAddress returns the address of a function inside dll ("vkCreateInstance")
  PFN_vkCreateInstance pfnVkCreateInstance = (PFN_vkCreateInstance)GetProcAddress(Engine::vkLib, "vkCreateInstance");
  if (pfnVkCreateInstance == nullptr)
  {
    ENGINE_ERROR("Failed to retrieve the address of vkCreateInstance");
    return EXIT_FAILURE;
  }

  uint32 ExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
  if (ExtensionCount <= 0)
  {
    return EXIT_FAILURE;
  }
  ENGINE_LOG("Number of Vulkan extensions:", ExtensionCount);
  ENGINE_LOG("vulkan-1.dll is successfully loaded");
#endif
  return true;
}

static bool DestroyVulkan()
{
  FreeLibrary(Engine::vkLib);
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
  ENGINE_LOG("Engine should close");
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
    ENGINE_ERROR("Failed to create window");
    TerminateEngine();
    return false;
  }
  ENGINE_LOG("Engine should start");
  Engine::IsAppRunning = true;
  glfwSetKeyCallback(Engine::Window::AppWindow, OnKeyPressed);

  if (CreateVulkan() == false)
  {
    ENGINE_ERROR("Failed to create renderer");
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