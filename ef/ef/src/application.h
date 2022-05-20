#pragma once

#include "src/engine/core.h"
#include <src/engine/native_engine.hpp>

namespace ef
{
	class EF_API Application
	{


	public:

		Application();
		virtual ~Application();

		void run(struct android_app *app);

		void print();

		static void externalStartCallback();


	};

	Application* createApplication();

	// To be defined in client

    // StartApp(struct android_app *app);
}