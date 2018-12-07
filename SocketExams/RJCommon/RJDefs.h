#pragma once

#ifdef WIN
#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif // !WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include <thread>

#ifndef interface
#define interface struct
#endif

//#define LOGENABLE
#ifdef LOGENABLE
#define LOG(x) x
#else
#define LOG(x) //x
#endif

#ifdef STDOUTENABLE
#define STDOUT(x) x
#else
#define STDOUT(x) //x
#endif

#ifdef WIN
#define Strcpy strcpy_s
#define Strcat strcat_s
#define Sprintf sprintf_s
#define Snprintf sprintf_s
#else
#include <string.h>
#define Strcpy strcpy
#define Strcat strcat
#define Sprintf sprintf
#define Snprintf snprintf
#endif

typedef std::thread RJThread;
typedef int			RJBOOL;
#define RJVOID		void
typedef void  *		RJLPVOID;
typedef void  *		RJHANDLE;
#define	RJNULL		0
#define RJTRUE		1
#define RJFALSE		0

#define RJLOCKED	1
#define RJUNLOCK	0

#ifdef WIN
typedef unsigned long       RJDWORD;
typedef long				RJLONG32;
typedef _int64				RJLONGLONG;
#else
typedef long long           RJLONGLONG;
typedef unsigned int		RJDWORD;
typedef int					RJLONG32;
#endif

typedef  RJLONG32 RJHRESULT;

#ifdef WIN
#define RJINT64 INT64
#define RJUINT64 UINT64
#else
#define RJINT64 long long
#define RJUINT64 unsigned long long
#endif

#define RJ_S_OK		(RJHRESULT)0x00000000L   ///< Succeeded 
#define RJ_NOERROR	(RJHRESULT)0x00000000L   ///< Succeeded 
#define RJ_S_FALSE	(RJHRESULT)0x00000001L   ///< Succeeded 

#define RJ_E_FAIL	((RJHRESULT)-1)   ///< Unknown Fail 

#define RJ_S_TIMEOUT	((RJHRESULT)-2)   ///< Timeout

#define RJ_INFINITE (RJHRESULT)0xFFFFFFFF  ///< Infinite timeout


#define RJ_E_TIMEOUT				((RJHRESULT)-13)   ///< Timeout 


