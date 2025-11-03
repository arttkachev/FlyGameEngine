#pragma once

#include <iostream>
#include <sstream>
#include "GLFW/glfw3.h" // a lib to create window, open, manage input and create contexts

using int32 = int32_t;
using uint32 = uint32_t;
using int16 = int16_t;
using uint16 = uint16_t;

// ptr to a main window
// create a window. We moved to global data to have an access to our main window from different scopes and functions 
// we moved its creation from a main loop
GLFWwindow* Window = nullptr;

// common renderer settings
const char* APP_TITLE = "Flyeng 1.0";
uint16 WindowWidth = 1440;
uint16 WindowHeight = 1080;
bool FullScreen = false;
bool DrawStats = true;
bool WireframeMode = false;
bool UseFPSCamera = false;


// glfw handlers
// callback function for specific key bindings
void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode);

// callback function on window resized
void OnFrameWindowBufferResized(GLFWwindow* window, int width, int height);

// a function to show frame stats
void ShowFrameStats(GLFWwindow* window, bool drawStats);

// a function to init OpenGL/Window
bool InitWindow();


int main() // entry renderer point
{
  // Init OpenGL
  if (!InitWindow())
  {
    std::cerr << "OpenGL initialization failed!" << std::endl;
    return -1;
  }

  // Main loop - window on the screen
  // While a method doesn't return true we get the window on the screen
  while (!glfwWindowShouldClose(Window))
  {
    // Stats
    ShowFrameStats(Window, DrawStats);

    // Quering any inputs (from keyboard, mouse and etc...)
    glfwPollEvents();

    // What kind of things we want to clear (in our case this is COLOR_BUFFER | DEPTH_BUFFER)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Double buffering (Front buffer is what a monitor shows, back buffer is what a video card draws. They are swapping to eliminate tearing)
    glfwSwapBuffers(Window);
  }

  // GLFW cleans up itself properly
  glfwTerminate();
  return 0;
}

// Callback function to escape the window
void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  // close window
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  // draw stats
  if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
  {
    DrawStats = !DrawStats;
  }

  // switch to wireframe mode
  if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
  {
    WireframeMode = !WireframeMode;
    if (WireframeMode)
    {
      // wireframe
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
      // fill
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  // switch camera
  if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
  {
    UseFPSCamera = false;
  }

  if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
  {
    UseFPSCamera = true;
  }
}

void OnFrameWindowBufferResized(GLFWwindow* window, int width, int height)
{
  // reset variables that specify our window size
  WindowWidth = width;
  WindowHeight = height;
  // a method that converts from OpenGL 2D coord to Screen viewport and resize our viewport
  // args (lower left corner of the coords (x), lower left corner of the coords (y), screen width (1024), screen height (768))
  glViewport(0, 0, WindowWidth, WindowHeight);
}

// show frame stats impl
void ShowFrameStats(GLFWwindow* window, bool drawStats)
{
  if (drawStats)
  {
    // this function counts all sort of stats
    static double previousSeconds = 0.0f;
    static uint16_t FrameCount = 0;
    double elapsedSeconds = 0.0f;
    double currentSeconds = glfwGetTime(); // retunrs number of seconds since GLFW started, as a double
    elapsedSeconds = currentSeconds - previousSeconds; // calculate elapsed seconds with simple math
    const float statUpdateTimeStep = 0.25f; // Limit data update by 4 times per second

    if (elapsedSeconds > statUpdateTimeStep)
    {
      previousSeconds = currentSeconds;
      double fps = static_cast<double>(FrameCount / elapsedSeconds);

      // A time it takes to render a frame
      constexpr float ms = 1000.0f;
      double msPerFrame = ms / fps;

      // output stats data with string stream (a standard C++ way)
      // TODO: a better implementation for stats output. This is not a better solution because of slight performance overhead
      std::ostringstream outs{};
      outs.precision(3); // set precision of number shown
      // print data
      outs << std::fixed // fixed precision
        // printing out any sort of stats information
        << APP_TITLE << "    "
        << "FPS: " << fps << "    "
        << "Frame Time: " << msPerFrame << " (ms)";

      // affect the title of this specific window (window argument) ARGS: (specific window to affect the title, output string for a title)
      glfwSetWindowTitle(window, outs.str().c_str());
      FrameCount = 0; //update data zero frame count again
    }
    FrameCount++; // count frames again
  }
  else
  {
    glfwSetWindowTitle(window, APP_TITLE);
  }
}

// init window impl (ex InitOpenGL)
bool InitWindow()
{
  // GLFW Initialization
  if (!glfwInit())
  {
    std::cerr << "GLFW Initialization failed" << std::endl;
    return false;
  }
  // Specify a window. OpenGL is cross-platform. We use an additional lib GLFW for window creation and inputs
  // Specify a max version of OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // Specify a min version of OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Specify OpenGL Core Profile (we won't support old methodes from OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // We support only version 3 and latest (forward) 
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  if (FullScreen)
  {
    // Let's think we only have one monitor
    // Get a pointer to the user's monitor device
    if (GLFWmonitor* Monitor = glfwGetPrimaryMonitor())
    {
      // Get a pointer to its current mode to know current resolution
      const GLFWvidmode* VideoMode = glfwGetVideoMode(Monitor);
      if (VideoMode != nullptr)
      {
        Window = glfwCreateWindow(VideoMode->width, VideoMode->height, APP_TITLE, Monitor, nullptr);
      }
    }
  }
  else
  {
    Window = glfwCreateWindow(WindowWidth, WindowHeight, APP_TITLE, nullptr, nullptr);
  }

  if (Window == nullptr)
  {
    std::cerr << "Failed to create a GLFW Window" << std::endl;
    // GLFW cleans up itself properly
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(Window); // Make our window as current (without that we can't draw)
  glfwSetKeyCallback(Window, OnKeyPressed); // Register a callback function

  // OpenGL function to clear a screen with a color every frame (the color that we use to clear a screen)
  // because OpenGL is a state machine we don't need to call glClearColor() every frame
  // we set it once and it works like a state machine, so we moved this call to here from a main loop
  // we're gonna have initOpenGL() call only once
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  glfwSetCursorPos(Window, WindowWidth / 2.0, WindowHeight / 2.0); // set cursor position in the center of the screen on init
  return true;
}