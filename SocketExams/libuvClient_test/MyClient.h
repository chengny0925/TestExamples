#pragma once
#include <RJTcpClient.h>
#include <map>
#include <RJSemaphore.h>
#include <RJRequestIdHelper.h>
#include <RJSysUtil.h>
#include <RJLock.h>

using namespace std;

//struct MsgBuf
//{
//	char* base;
//	int len;
//};

#define REQ_ID_LENGH 9
#define CMD_LENGTH 31
#define MSG_HEAD_LENGTH (REQ_ID_LENGH + CMD_LENGTH)

#define SERVER_HEARTBEAT "HEARTBEAT"
#define SERVER_CMD_SUBSCRIBE_COMMAND "SUBSCRIBE_COMMAND"
#define SERVER_CMD_UNSUBSCRIBE_COMMAND "UNSUBSCRIBE_COMMAND"
#define SERVER_CMD_STX_COMMAND "STX_COMMAND"

#define SERVER_CMD_STX_START_INFO "STX_START_INFO"
#define SERVER_CMD_STX_ORDER "STX_ORDER"
#define SERVER_CMD_RT_POSITION "RT_POSITION"

struct AsyncSendRtn
{
	int errorCode;
	int reqId;
};

struct SyncSendRtn
{
	int errorCode;
	string cmd;
	string value;
};

struct IClientCallBack
{
	virtual ~IClientCallBack() {}
	virtual void OnConnection() = 0;
	virtual void OnDisConnection() = 0;
	virtual void OnError() = 0;
	virtual void OnMsg(int reqId, string& cmd, string& msg) = 0;
}; 

struct IClient
{
	virtual ~IClient() {}
	virtual int Start(const char* ip, int port) = 0;
	virtual void SetClientCallBack(IClientCallBack* callBack) = 0;
	virtual AsyncSendRtn AsyncSend(const char * cmd, const char* msg, int size) = 0;
	virtual SyncSendRtn SyncSend(const char * cmd, const char* msg, int size, int ms) = 0;
};


class MyClient : public IClient
{
public:
	MyClient();
	~MyClient();

	int Start(const char* ip, int port);
	void SetClientCallBack(IClientCallBack* callBack);
	int SyncCallmethod();

	AsyncSendRtn AsyncSend(const char * cmd, const char* msg, int size);
	SyncSendRtn SyncSend(const char * cmd, const char* msg, int size, int ms = 1000);

private:
	AsyncSendRtn Send(const char * cmd, const char* msg, int size, int reqId = -1);
	void func(int msg_type, const char* msg, int len);
	void OnMsg(const char* msg, int len);
	void OnConnection();
	void OnError();
	void OnDisConnection();

	void MsgDespatcher();
	void Handle(string& msg);
	RJSemaphore* GetSyscReqSem(int reqId);

private:
	RJTcpClient m_client;
	IClientCallBack* m_callBack;

	RJLock m_reqSemLock;
	map<int, RJSemaphore*> m_reqSemMap;
	map<int, string> m_reqResultMap;
	RJRequestIdHelper m_requestIdHelper;

	RJSemaphore m_receiveMsgSem;
	RJLock m_receiveMsgLock;
	std::vector<string> m_receiveMsgVec;

	std::thread* m_pThread;
	bool m_is_running;
	//bool m_has_new_msg;
};

