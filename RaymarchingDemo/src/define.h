//------------------------------------------------------------------------------------------
//! @file	define.h
//! @brief	共通定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------
#pragma once

#if !defined(DEBUG) && !defined(_DEBUG)
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )	// コンソールウインドウを非表示
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif
#ifndef _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1
#endif

#include <crtdbg.h>
#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif


#define PI 3.1415926535f

template<class T> static inline T RadToDeg(T rad) { return ( (rad)*(180.0f/PI) ); }
template<class T> static inline T DegToRad(T deg) { return ( (deg)*(PI/180.0f) ); }

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if(x) { delete (x); (x)=nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if(x) { delete[] (x); (x)=nullptr; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x)=nullptr; } }
#endif

#ifndef DLOG
#if defined(DEBUG) || defined(_DEBUG)
#define DLOG(x, ...) std::printf( "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ );
#else
#define DLOG(x, ...)
#endif
#endif

#ifndef ELOG
#define ELOG(x, ...) std::printf( "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ );
#endif

#ifndef ASSERT
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT(b, x, ...) if(!(b)) { std::printf( "[ assert ] [File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ ); abort(); }
#else
#define ASSERT(x, ...)
#endif
#endif

#ifndef WARNING
#if defined(DEBUG) || defined(_DEBUG)
#define WARNING(b, x, ...) if(!(b)) { std::printf( "[ warning ] [File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ ); _CrtDbgBreak(); }
#else
#define WARNING(x, ...)
#endif
#endif
