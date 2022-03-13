#include "application.h"
#include <native_engine.hpp>

#include <iostream>

//#include "glad/glad.h"

namespace ef
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::run()
	{
/*
		SDL_Window* window;
		SDL_Init(SDL_INIT_VIDEO);

		window = SDL_CreateWindow(
			"An SDL2 window",                  // window title
			SDL_WINDOWPOS_UNDEFINED,           // initial x position
			SDL_WINDOWPOS_UNDEFINED,           // initial y position
			640,                               // width, in pixels
			480,                               // height, in pixels
			SDL_WINDOW_OPENGL                  // flags - see below
		);


		// Check that the window was successfully created
		if (window == NULL) {
			// In the case that the window could not be made...
			//printf("Could not create window: %s\n", SDL_GetError());
            printf("Could not create window");
			return;
		}
*/
		//while (1)
		//{
//
		//}
	}
	void Application::print()
	{

        
	}

	void Application::externalStartCallback() {

	}

}

extern "C" {
void android_main(struct android_app *app);
}

/*
    android_main (not main) is our game entry function, it is called from
    the native app glue utility code as part of the onCreate handler.
*/

void android_main(struct android_app *app){

    NativeEngine *engine = new NativeEngine(app);
    engine->GameLoop();
    delete engine;
}