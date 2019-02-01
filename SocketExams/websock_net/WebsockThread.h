#ifndef WEBSOCKTHREAD_H
#define WEBSOCKTHREAD_H

#include "websock_net_global.h"

class WEBSOCK_NET_CLASS WebsockThread
{
public:
	enum ThreadPriority
	{
		TimeCritical,   //  ������ȼ�
		Highest,
		AboveNormal,
		Normal,
		BelowNormal,
		Lowest,
		Idle              //  ������ȼ�
	};
public:
	WebsockThread(ThreadPriority prior = Normal);
	virtual ~WebsockThread();
	bool start();             //  ����

	void Join();
	void Detach();
	void Stop();
	//////////////////////////////////////////////////////
protected:
	virtual int run() = 0;      //  �̺߳���
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