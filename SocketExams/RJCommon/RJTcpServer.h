#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include "LibUvHelper.h"

//#include "RJDefs.h"


#define DEFAULT_IP "0.0.0.0"
#define BACK_LOG 128

enum class uv_send_type
{
	Single = 0,
	//Broadcast
};

typedef struct
{
	uv_tcp_t client;
	BufferReader reader;
}uv_tcp_client;

class IRJTcpServerCallBack
{
public:
	virtual ~IRJTcpServerCallBack() {};
	virtual void OnMsg(uv_tcp_client* client, const char* msg, int size) = 0;
	virtual void OnNewConnection(uv_tcp_client* client) = 0;
	virtual void OnDisconnection(uv_tcp_client* client) = 0;
	virtual void OnLog(const char* log);
};

typedef struct
{
	uv_tcp_t* client;
	uv_buf_t buf;
	uv_send_type type;
}uv_tcp_send_buf;

class RJTcpServer
{
public:
	RJTcpServer();
	~RJTcpServer();

private:
	std::vector<uv_tcp_client*> m_clients;
	std::vector<uv_tcp_client*> m_clients4delete;
	bool m_is_closing;
	uv_loop_t* m_pLoop;
	uv_tcp_t m_server;
	uv_async_t m_async_handle;
	std::thread* m_p_thread;
	std::queue<uv_tcp_send_buf> m_send_buf;
	std::mutex m_send_buf_lock;

public:
	IRJTcpServerCallBack* m_callback;

public:
	void Close();
	int Init(int port);
	int Send(uv_tcp_t* client, const char* msg, int size);
	//void Broadcast(const char* msg, int size);

private:
	void OnMsg(uv_tcp_t* client, const char* msg, int size);
	void OnNewConnection(uv_tcp_client* client);
	void OnDisconnection(uv_tcp_client* client);


private:
	void SendLog(const char* source, const char* errorMsg);
	void RunThread();
	int RemoveClient(uv_tcp_t* client);
	bool CheckClient(uv_tcp_t* client);
	static void DeleteWriteReq(uv_write_t* write);
	static void AfterSend(uv_write_t* req, int status);
	//static void AfterBroadcast(uv_write_t* req, int status);
	static void AllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void HandleMsg(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void ClientClose(uv_handle_t* handle);
	static void AcceptConnection(uv_stream_t* server, int status);
	static void AsyncCallBack(uv_async_t* handle);
	static void WalkCallBack(uv_handle_t* handle, void* arg);
};

