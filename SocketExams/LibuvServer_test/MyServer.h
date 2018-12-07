#pragma once
#include <RJTcpServer.h>
#include <string>
#include <RJSpinLock.h>

using namespace std;

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

struct ServerBufferStruct
{
	uv_tcp_client* client;
	std::string msg;
};

class MyServer : public IRJTcpServerCallBack
{
public:
	MyServer();
	~MyServer();

	virtual void OnMsg(uv_tcp_client* client, const char* msg, int size);

	void EchoCmd(uv_tcp_client* client, int id, string& cmd, string& value);

	virtual void OnNewConnection(uv_tcp_client* client);
	virtual void OnDisconnection(uv_tcp_client* client);

	virtual void OnLog(const char* log)
	{
		cout << "OnLog " << log << endl;
	};

public:
	void Init(int port)
	{
		m_server->Init(port);
	}
	void Broadcast(char* msg, int length)
	{
		for (auto& it : m_clients)
		{
			m_server->Send(&it->client, msg, length);
		}
	}

private:
	void MsgDespatcher();
	void DespatchMsg(ServerBufferStruct& msg);

private:
	RJSpinLock m_clientsLock;
	std::vector<uv_tcp_client*> m_clients;
	RJTcpServer* m_server;

	std::thread* m_pThread;
	std::vector<ServerBufferStruct> m_bufs;
	std::mutex m_bufs_lock;
	bool m_is_running;
	bool m_has_new_msg;
};
