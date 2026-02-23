#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
using namespace std;


class Display
{
public:
	Display();
	~Display();
	void initDisplay();
	void swapBuffer();
	void clearDisplay(float r, float g, float b, float a);
	int* GetHeight() {
		return (int*) &_screenHeight;
	};

	int* GetWidth() {
		return (int*) &_screenWidth;
	};

private:

	void returnError(std::string errorString);

	SDL_Window* _window; //holds pointer to out window
	float _screenWidth;
	float _screenHeight;
};
