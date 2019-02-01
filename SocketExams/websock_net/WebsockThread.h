#ifndef WEBSOCKTHREAD_H
#define WEBSOCKTHREAD_H

#include "websock_net_global.h"

class WEBSOCK_NET_CLASS WebsockThread
{
public:
	enum ThreadPriority
	{
		TimeCritical,   //  最高优先级
		Highest,
		AboveNormal,
		Normal,
		BelowNormal,
		Lowest,
		Idle              //  最低优先级
	};
public:
	WebsockThread(ThreadPriority prior = Normal);
	virtual ~WebsockThread();
	bool start();             //  启动

	void Join();
	void Detach();
	void Stop();
	//////////////////////////////////////////////////////
protected:
	virtual int run() = 0;      //  线程函数
protected:
	bool stopped_;
	//////////////////////////////////////////////////////
private:
	static unsigned __stdcall __entry(void *);

private:
	ThreadPriority thread_priority_;
	void *thread_hanlde_;
	unsigned int   thread_id_;

};

#endif  // WEBSOCKTHREAD_H