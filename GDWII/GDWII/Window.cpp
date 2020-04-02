#include "Window.h"

Window::Window(const std::string title, int windowWidth, int windowHeight, bool resizable)
{
	//Window is now open
	m_open = true;

	//Create the window
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	//If window is NULL
		//Give failed window creation error message
	//Else
		//Set up the GL context
	if (m_window == NULL)
	{
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	else
	{
		//Create GL context
		m_context = SDL_GL_CreateContext(m_window);
		SetWindowResizable(resizable);

		//Check to make sure the GL context was created properly
		if (m_context == NULL)
		{
			std::cerr << "Failed to create GL context!" << std::endl;

			Close();
		}
	}
}

Window::~Window()
{
	//If context isn't NUll
	if (m_context != NULL)
	{
		//delete context
		SDL_GL_DeleteContext(m_context);
		//Set context to NULL
		m_context = NULL;
	}

	//If window isn't nullptr
	if (m_window != nullptr)
	{
		//Delete window
		SDL_DestroyWindow(m_window);
		//Set window equal to nullptr
		m_window = nullptr;
	}
}

void Window::Clear(float r, float g, float b, float a)
{
	//Set the clear color
	glClearColor(r, g, b, a);
	//Clear the color buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Clear(vec4 color)
{
	//Set the clear color
	glClearColor(color.x, color.y, color.z, color.w);
	//Clear the color buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Flip()
{
	//Swaps the buffers (using double buffer system)
	SDL_GL_SwapWindow(m_window);
 }

void Window::SetWindowName(std::string name)
{
	SDL_SetWindowTitle(m_window, name.c_str());
}

void Window::SetWindowResizable(bool resizable)
{
	//Sets the window resizable
	SDL_SetWindowResizable(m_window, SDL_bool(resizable));
}

void Window::SetFullscreen(Uint32 flags)
{
	//Sets the window fullscreen flag
	SDL_SetWindowFullscreen(m_window, flags);
	m_fullscreen = flags;
}

void Window::SetBorderless(bool borderless) {
	SDL_SetWindowBordered(m_window, SDL_bool(borderless));
}

void Window::SetWindowPos(int x, int y) {
	SDL_SetWindowPosition(m_window, x, y);
}

Uint32 Window::GetFullscreen() const
{
	//Return window fullscreen flag
	return m_fullscreen;
}

int* Window::GetWindowSize() {
	int winSize[] = {0, 0};

	SDL_GetWindowSize(m_window, &winSize[0], &winSize[1]);
	return winSize;
}

int* Window::GetWindowPos() {
	int winPos[] = { 0, 0 };

	SDL_GetWindowPosition(m_window, &winPos[0], &winPos[1]);
	return winPos;
}

void Window::SetWindowSize(int w, int h)
{
	SDL_SetWindowSize(m_window, w, h);

	int* winPos = GetWindowPos();
	SDL_SetWindowPosition(m_window, winPos[0], winPos[1]);
}

void Window::Close()
{
	//Set window open to false
	m_open = false;
}
