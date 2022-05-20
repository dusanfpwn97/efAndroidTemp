#pragma once

//#ifdef EF_PLATFORM_WINDOWS

extern ef::Application* ef::createApplication();

int main(int argc, char** argv)
{
	//int s[] = { 1, 3 ,85 };

	auto app = ef::createApplication();

	app->run();

	delete app;
}