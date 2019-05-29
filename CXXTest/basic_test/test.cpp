#include "test.h"
#include <thread>
#include <sstream>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/syscall.h>  
#define gettidv1() syscall(__NR_gettid)  
#define gettidv2() syscall(SYS_gettid) 
#endif

using namespace std;

int getThreadId()
{
	int tid;
#ifdef _WIN32
	tid = GetCurrentThreadId();
#else
	tid = gettidv1();
#endif
	return tid;
}

int getThreadId2()
{
	auto id = std::this_thread::get_id();
	ostringstream oss;
	oss << id;
	std::string stid = oss.str();
	unsigned long long tid = std::stoull(stid);
	return tid;
}

