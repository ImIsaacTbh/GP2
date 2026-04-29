#include "Display.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl2.h"
#include "IMGUI/imgui_impl_opengl3.h"

Display::Display()
{
	_window = nullptr; //initialise to generate null access violation for debugging. 
	_screenWidth = 1706;
	_screenHeight = 960; 
}

Display::~Display()
{
}

void Display::returnError(std::string errorString)
{
	std::cout << (errorString + "\n" + "Press Any Key To Quit...");
	exit(0);
}

void Display::swapBuffer()
{
	SDL_GL_SwapWindow(_window);
}

void Display::clearDisplay(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor

}

void Display::initDisplay()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //Min no of bits used to diplay colour
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // set up double buffer   
	_window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)_screenWidth, (int)_screenHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(_window);
	GLenum error = glewInit();
	if (_window == nullptr) returnError("Failed to create window.");
	if (context == nullptr) returnError("Failed to create openGL context.");
	if (error != GLEW_OK) returnError("Failed to initalise GLEW.");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.13f, 0.6f, 0.71f, 0.0f);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(_window, context);
	ImGui_ImplOpenGL3_Init();

	std::cout << glGetString(GL_VERSION) << std::endl;
}