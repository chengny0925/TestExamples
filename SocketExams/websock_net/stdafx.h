// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here

#define LOGGER(x,name)     static log4cxx::LoggerPtr x = log4cxx::Logger::getLogger(name)
#define LOG_VAR(x)         #x "[" << x << "]"