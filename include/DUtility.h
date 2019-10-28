#ifndef _DUTILITY_HEADER_
#define _DUTILITY_HEADER_


#ifdef _WIN32
	#ifdef DU_DLL_PROVIDER
		#define  DU_DLL_API _declspec(dllexport)
	#else
		#define  DU_DLL_API _declspec(dllimport)
	#endif
#else
	#define  DU_DLL_API
#endif


#include <chrono>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#endif// 2018/10/29