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

static bool CreateVulkan()
{
#if defined WinPlatform
  // dynamically load vulkan lib
  VulkanSpecs::Vk_Lib = LoadLibraryA("vulkan-1.dll"); // HMODULE holds a base address of dll
  if (VulkanSpecs::Vk_Lib == nullptr)
  {
    ENGINE_ERROR("vulkan-1.dll is failed to load");
    return false;
  }

  // GetProcAddress returns the address of a function inside dll ("vkCreateInstance")
  PFN_vkCreateInstance Vk_CreateInstance = (PFN_vkCreateInstance)GetProcAddress(VulkanSpecs::Vk_Lib, "vkCreateInstance");
  if (Vk_CreateInstance == nullptr)
  {
    ENGINE_ERROR("vkGetInstanceProcAddr failed");
    return false;
  }

  PFN_vkEnumerateInstanceVersion Vk_EnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)GetProcAddress(VulkanSpecs::Vk_Lib, "vkEnumerateInstanceVersion");
  if (Vk_EnumerateInstanceVersion == nullptr)
  {
    ENGINE_ERROR("vkGetInstanceProcAddr failed");
    return false;
  }

  if (vkEnumerateInstanceVersion(&VulkanSpecs::ApiVersion) != VK_SUCCESS)
  {
    ENGINE_ERROR("vkEnumerateInstanceVersion failed");
    return false;
  }

  const char* InstanceLayers[] = { "VK_LAYER_KHRONOS_validation" };
  const char* InstanceExtensions[] = { "" };

  VkApplicationInfo Vk_ApplicationInfo =
  {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,
    nullptr,
    Engine::Window::Title,
    VulkanSpecs::AppVersion,
    nullptr,
    0,
    VulkanSpecs::ApiVersion
  };

  VkInstanceCreateInfo Vk_InstanceCreateInfo =
  {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    nullptr,
    0,
    &Vk_ApplicationInfo,
    sizeof(InstanceLayers) / sizeof(InstanceLayers[0]),
    InstanceLayers,
    sizeof(InstanceExtensions) / sizeof(InstanceExtensions[0]),
    InstanceExtensions
  };

  /*if (Vk_CreateInstance(&Vk_InstanceCreateInfo, nullptr, &Engine::Vk_Instance) != VK_SUCCESS)
  {
    ENGINE_ERROR("vkCreateInstance is failed");
    return false;
  }*/

  uint32 ExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
  if (ExtensionCount <= 0)
  {
    return false;
  }
  ENGINE_LOG("Number of Vulkan extensions:", ExtensionCount);
  ENGINE_LOG("vulkan-1.dll is successfully loaded");
#endif
  return true;
}

static bool DestroyVulkan()
{
  FreeLibrary(VulkanSpecs::Vk_Lib);
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
    Engine::Window::AppWindow = glfwCreateWindow(ViewMode->width, ViewMode->height, Engine::Window::Title, nullptr, nullptr);
  }
  else
  {
    Engine::Window::AppWindow = glfwCreateWindow(Engine::Window::Width, Engine::Window::Height, Engine::Window::Title, nullptr, nullptr);
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
    glfwSetWindowShouldClose(Engine::Window::AppWindow, 1);
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
  while (Engine::IsAppRunning == true && glfwWindowShouldClose(Engine::Window::AppWindow) == 0)
  {
    RunMainLoop();
  }
  TerminateEngine();
}


int main()
{
  if (InitEngine() == true)
  {
    RunEngine();
  }
  return 0;
}