#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h" // to work with openGL and have access to all available method on a specific video card
#include "GLFW/glfw3.h" // a lib to create, open, manage input and create contexts

// ptr to a main window
// create a window. We moved to global data to have an access to our main window from different scopes and functions 
// we moved its creation from a main loop
GLFWwindow* gWindow = nullptr;

// common renderer settings
const char* APP_TITLE = "FlyEngine";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
const bool gFullScreen = false;
bool gDrawStats = false;

// callback function for specific key bindings
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);

// a function to show frame stats
void showFrameStats(GLFWwindow* window, bool drawStats);

// a function to init OpenGL
bool initOpenGL();


int main() // entry renderer point
{
  // Init OpenGL
  if (!initOpenGL())
  {
    std::cerr << "OpenGL initialization failed!" << std::endl;
    return -1;
  }

	// Main loop - window on the screen
	// While a method doesn't return true we get the window on the screen
	while (!glfwWindowShouldClose(gWindow))
	{
		// Stats
		showFrameStats(gWindow, gDrawStats);
		
		// Quering any inputs (from keyboard, mouse and etc...)
		glfwPollEvents();		

		// What kind of things we want to clear (in our case this is COLOR_BUFFER)
		glClear(GL_COLOR_BUFFER_BIT);

		// Double buffering (Front buffer is what a monitor shows, back buffer is what a video card draws. They are swapping to eliminate tearing)
		glfwSwapBuffers(gWindow);
	}

	// GLFW cleans up itself properly
	glfwTerminate();
	return 0;
}

// Implementation of functions

// Callback function to escape the window
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		gDrawStats = !gDrawStats;
	}
}

 // show fram stats implementation
void showFrameStats(GLFWwindow* window, bool drawStats)
{	
	if (drawStats)
	{
		// this function counts all sort of stats
		static double previousSeconds = 0.0;
		static int FrameCount = 0;
		double elapsedSeconds;
		double currentSeconds = glfwGetTime(); // retunrs number of seconds since GLFW started, as a double

											   // calculate elapsed seconds with simple math
		elapsedSeconds = currentSeconds - previousSeconds;

		// Limit data update by 4 times per second
		const float updateLimitValue = 0.25;

		if (elapsedSeconds > updateLimitValue)
		{
			previousSeconds = currentSeconds;
			double fps = static_cast<double>(FrameCount / elapsedSeconds);

			// A time it takes to render a frame
			const float ms = 1000.0;
			double msPerFrame = ms / fps;

			// output stats data with string stream (a standard C++ way)
			// TODO: a better implementation for stats output. This is not a better solution because of slight performance overhead
			std::ostringstream outs;

			// set precision of number shown
			outs.precision(3);

			// print data
			outs << std::fixed // fixed precision
							   // printing out any sort of stats information
				<< APP_TITLE << "    "
				<< "FPS: " << fps << "    "
				<< "Frame Time: " << msPerFrame << " (ms)";

			// affect the title of this specific window (window argument) ARGS: (specific window to affect the title, output string for a title)
			glfwSetWindowTitle(window, outs.str().c_str());

			// update data
			// zero frame count again
			FrameCount = 0;
		}

		// count frames again
		FrameCount++;
	}

	else
	{		
		glfwSetWindowTitle(window, APP_TITLE);
	}
	
}

// init OpenGL() implementation
bool initOpenGL()
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
  
  if (gFullScreen)
  {
    // Let's think we only have one monitor
    // Get a pointer to the user's monitor device
    if (GLFWmonitor* pMonitor = glfwGetPrimaryMonitor())
    {
      // Get a pointer to its current mode to know current resolution
      const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
      if (pVmode != nullptr)
      {
        gWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, nullptr);
      }
    }
  }

  else
  {
    gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, nullptr, nullptr);
  }

  if (gWindow == nullptr)
  {
    std::cerr << "Failed to create a GLFW Window" << std::endl;
    // GLFW cleans up itself properly
    glfwTerminate();
    return false;
  }
  // Make our window as current (without that we can't draw)
  glfwMakeContextCurrent(gWindow);

  // Register a callback function
  glfwSetKeyCallback(gWindow, glfw_onKey);

  // We need to initialize that (according to documentation) to set GLEW properly 
  glewExperimental = GL_TRUE;

  // We initialize a GLEW lib that gives us an access to OpenGL (We get pointers to all available methods available on a specific video card that we have) 
  if (glewInit() != GLEW_OK)
  {
    std::cerr << "GLEW Initialization failed" << std::endl;
    return false;
  }
  
  // OpenGL function to clear a screen with a color every frame (the color that we use to clear a screen)
  // because OpenGL is a state machine we don't need to call glClearColor() every frame
  // we set it once and it works like a state machine, so we moved this call to here from a main loop
  // we're gonna have initOpenGL() call only once
  glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
  return true;

}