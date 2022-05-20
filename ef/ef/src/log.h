#pragma once
#include "src/engine/core.h"
#include <string>

namespace ef
{
	class EF_API Log
	{
	enum LogType
	{
		LOG,
		WARN,
		ERR,
		FATAL
	};


	public:
		
		static void init();

		template <class T>
		static void printLog(T t)
		{
			//std::cout << t;
		}

		template <class T, class... Args>
		static void printLog(T t, Args... args)
		{
			//std::cout << t << std::endl;

			print(args...);
		}

		//static void PrintLog(auto varToPrint);
		//static void PrintLog(int varToPrint);
		//static void PrintLog(float varToPrint);

	};


}

#define EFLOG(...) ::ef::Log::printLog(__VA_ARGS__)