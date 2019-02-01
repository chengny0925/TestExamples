#ifndef WEBSOCK_NET_GLOBAL_H
#define WEBSOCK_NET_GLOBAL_H

//#ifndef  WEBSOCK_NET_EXPORTS
//#	define  WEBSOCK_NET_IMPORT  __declspec(dllimport)
//#else
//#	define  WEBSOCK_NET_IMPORT  __declspec(dllexport)
//#endif

#ifdef _WIN32
#ifdef WEBSOCK_NET_EXPORTS
#define WEBSOCK_NET_CLASS   __declspec(dllexport)
#define WEBSOCK_NET_FUN     extern "C" __declspec(dllexport)
#else
#define WEBSOCK_NET_CLASS   __declspec(dllimport)
#define WEBSOCK_NET_FUN     extern "C" __declspec(dllimport)
#endif


#else // LINUX
#define WEBSOCK_NET_CLASS 
#define WEBSOCK_NET_FUN extern "C"

#endif

#endif // WEBSOCK_NET_GLOBAL_H



