#pragma once

#include "core.h"


namespace ef
{
	class EF_API Application
	{


	public:

		Application();
		virtual ~Application();

		void run();

		void print();

		static void externalStartCallback();
	};


	// To be defined in client
	Application* createApplication();

    // StartApp(struct android_app *app);
}