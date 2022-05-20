#include "application.h"
#include <src/engine/native_engine.hpp>

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

	void Application::run(struct android_app *app)
	{

		NativeEngine *engine = new NativeEngine(app);
		engine->GameLoop();
		delete engine;

	}
	void Application::print()
	{

        
	}

	void Application::externalStartCallback() {

	}

}

