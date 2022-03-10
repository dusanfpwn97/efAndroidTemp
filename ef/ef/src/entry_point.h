#pragma once

//#ifdef EF_PLATFORM_WINDOWS
#include <string>
extern ef::Application* ef::createApplication();

int main(int argc, char** argv)
{
	//int s[] = { 1, 3 ,85 };
	
	auto app = ef::createApplication();
	
	app->run();

	delete app;
}

//#endif
/*
#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_eferus_ef_MainActivity_stringFromJNI2(
        JNIEnv* env,
        jobject this ) {
    std::string hello = "Hello 2 C++";
    return env->NewStringUTF(hello.c_str());
}*/