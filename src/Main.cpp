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
bool gWireframeMode = false;

// basically shaders are external files containing source code for loading
// but at this step to test a triangle we write a shader in our main.cpp file
// vertex shader
const GLchar* vertexShaderSrc =
"#version 330 core\n" // version of a shader model
"layout (location = 0) in vec3 pos;" // vec3 = type, pos = variable
//"layout (location = 1) in vec3 color;" // location = 1 coincides with 1 in  glVertexAttribPointer(1 - first parameter for color
//"out vec3 vert_color;" // we need to pass color information into a fragment shader
"void main()"
"{"
//"     vert_color = color;" // we assign to vert_color what we pass in the vertex shader (see above this declaration)
"     gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);" // just outputs vertices' position. gl_Position - output of vertex shader as vec4
"}";

// fragment shader
const GLchar* fragmentShaderSrc =
"#version 330 core\n"
//"in vec3 vert_color;" // input for color from a vertex shader (with the same name)
"out vec4 fragColor;" // output
"void main()"
"{"
"    fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);" // assign output color
"}";

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

  // create a triangle to draw it
  // it is just an array of floats in normalized coordinate space
  // it's responsibility of a vertext shader to convert vertax data from 3D space (on input) into normalized coord.space
  // GLfloat - typedef of a native float. It also works for other primitive types. We use the typedef because
  // a native float's size depends on different operating system, so it's a good idea to use OpenGL types it defines
  
  //position
  GLfloat quad[] = {   

   -0.5f, 0.5f, 0.0f,
    0.5f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
   -0.5f, -0.5f, 0.0f

  };

  // indices to prevent from double vertices to draw 2 triangles forming a quad
  GLuint indices[] = {
    0, 1, 2, // trig 0
    0, 2, 3  // trig 1
  };

  //// color
  //GLfloat vert_color[] = {
  //  1.0f, 0.0f, 0.0f,  // red
  //  0.0f, 1.0f, 0.0f,  // green
  //  0.0f, 0.0f, 1.0f   // blue
  //};

  // next we need to send triangle vertices to GPU
  // we use a spacicial object for thist called vertext buffer object - the buffer that holds data in GPU memory
  // sending data from CPU to GPU by a bus is time-consuming, so we need to minimize this process


  ////////////// VERTEX BUFFER OBJECT//////////////
  // our vertex buffer object identifier (uint)
  GLuint vbo{}; //, vbo_color{};
  GLuint ibo{}; // index buffer object

  // generate actual vertext buffer object
  // it creates a chunk of memory in the graphics card for us
  glGenBuffers(1, &vbo); //args: number of buffers, it returns back an identifer for the buffer through the variable vbo

  // makes a created buffer as a current buffer. Only one buffer at a time can be active in OpenGL
  glBindBuffer(GL_ARRAY_BUFFER, vbo); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  // fill our buffer with data
  // after these 3 calls above we created a buffer in GPU and copied our triangle data (vertices) to it
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW); // args: kind of buffer, its size, actural data, type of drawing (STATIC/DYNAMIC/STREAM)
  
  //// generate actual vertext buffer object
  //// it creates a chunk of memory in the graphics card for us
  //glGenBuffers(1, &vbo_color); //args: number of buffers, it returns back an identifer for the buffer through the variable vbo

  //// makes a created buffer as a current buffer. Only one buffer at a time can be active in OpenGL
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_color); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  //// fill our buffer with data
  //// after these 3 calls above we created a buffer in GPU and copied our triangle data (vertices) to it
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW); // args: kind of buffer, its size, actural data, type of drawing (STATIC/DYNAMIC/STREAM)

////////////// VERTEX ARRAY OBJECT//////////////
  // next we need to have vertext array object to draw that holds a vertex buffer object
  // its identifier
  GLuint vao{};
  
  // Gen vertex array object in the same fashion we generated a buffer above
  glGenVertexArrays(1, &vao); // number, bind identifier

  // make it an active vertex array object by binding it
  glBindVertexArray(vao);

  //// call it agian because only 1 buufer might be active at a given time. We ensure we work with an appropriate buffer next call
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_position); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  // we need to tell a vertext shader how to interpret a buffer (give it a format of vertices in memory (layout in memory))  
  // buffer is a just bytes
  // we do this with this call
  // IMPORTANT: before this call we need to have vao object bound

  //POSITION
  // layout for position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // args: attribute index (0 - position attribute index. it's specified by OpenGL),
  // number of elements in data (3 - specifies each vertex position in our triangle data), type of data, does it need to be normalized, 
  // stride (space between elements) = sizeof(GLfloat) * 6 because 6 elements before next position element in the interleaved layout, 
  // stride/offset (space before the first element) = nullptrt because it starts from 0 byte 

  // by default VertexAttrib is disabled in OpenGL. We need to enable it
  glEnableVertexAttribArray(0); //args: attrib index (0 = position) in our vertex attribute array

  //// call it agian because only 1 buufer might be active at a given time. We ensure we work with an appropriate buffer next call
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_color); // args: kind of buffer we wanna make active (array buffer because we have an array), its identifier) 

  //// COLOR
  //// layout for color
  //// last argument (GLvoid*)(sizeof(GLfloat) * 3) because the first color element starts at the 4th element
  //// in the interleaved layout
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  //// by default VertexAttrib is disabled in OpenGL. We need to enable it
  //glEnableVertexAttribArray(1); //args: attrib index (1 = color) in our vertex attribute array

  // set up the index buffer object (below vertex array setup) in the same way like other buffers
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // GL_ELEMENT_ARRAY_BUFFER means index buffer object in OpenGL
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  ////////////CREATE_SHADERS////////////////////

  // vertext shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 

  // assign vertex shader source
  glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr); // args: identifier, number of shaders, source code of shader, len of shader
  
  // compile vertex shader
  glCompileShader(vertexShader);

  // log error if vertex shader failed to compile
  GLint result{};
  GLchar infoLog[512];

  // get compile result
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

  if (!result)
  {
    // get log info
    glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
    std::cout << "Error! Vertex Shader failed to compile" << infoLog << std::endl;
  }

  // fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  // assign fragment shader source
  glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr); // args: identifier, number of shaders, source code of shader, len of shader

  // compile fragment shader
  glCompileShader(fragmentShader);

  // log error if fragment shader failed to compile
  // get compile result
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

  if (!result)
  {
    // get log info
    glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
    std::cout << "Error! Fragment Shader failed to compile" << infoLog << std::endl;
  }

  // shader program
  GLint shaderProgram = glCreateProgram();

  // attach our shaders to a created shader program 
  glAttachShader(shaderProgram, vertexShader); // args: shader program, attaching shader
  glAttachShader(shaderProgram, fragmentShader); // args: shader program, attaching shader

  // Link shader program
  glLinkProgram(shaderProgram);

  // log shader program
  // get link status info
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);

  if (!result)
  {
    // get info log
    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
    std::cout << "Error! Shader Program Linker failure!" << infoLog << std::endl;
  }

  // because we linked shaders to a shader program we don't need them anymore
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  ////////////END_CREATE_SHADERS////////////////////


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

    ////////////////DRAWING_TRIANGLE///////////////////////////
    // activate shader program
    glUseProgram(shaderProgram);

    // before each draw we must bind our vertext array object
    glBindVertexArray(vao);

    //// how to draw a triangle (wining order clockwise)
    //glDrawArrays(GL_TRIANGLES, 0, 6); // args: what kind of primitives our data makes up, start position, number of vertices (3 for a triangle / 6 for a quad)

    // since we use index buffer to prevent from double vertices in drawing quads we use another method different from glDrawArrays
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // args: what kind of primitives our data makes up, number of vertices, type for indices, offset (no stride in front of the array)

    // after each draw we must undind our vertext array object
    glBindVertexArray(0);
    ///////////////END_DRAWING_TRIANGLE///////////////////////

		// Double buffering (Front buffer is what a monitor shows, back buffer is what a video card draws. They are swapping to eliminate tearing)
		glfwSwapBuffers(gWindow);
	}

  // since we're done it's a good idea to clean up everything from memory
  // delete shader program
  glDeleteProgram(shaderProgram);

  // delete vertex array object
  glDeleteVertexArrays(1, &vao);

  // delete position vertex buffer object (separate buffer layout)
  glDeleteBuffers(1, &vbo);

  glDeleteRenderbuffers(1, &ibo); // num of buffers, what a buffer to delete

  //// delete color vertex buffer object (separate buffer layout)
  //glDeleteBuffers(1, &vbo_color);

	// GLFW cleans up itself properly
	glfwTerminate();
	return 0;
}

// Implementation of functions

// Callback function to escape the window
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  // close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

  // draw stats
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		gDrawStats = !gDrawStats;
	}

  // switch to wireframe mode
  if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
  {
    gWireframeMode = !gWireframeMode;
    if (gWireframeMode)
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