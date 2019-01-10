#pragma once
#include "uv.h"
#include <queue>
#include <mutex>
#include <thread>
#include "LibUvHelper.h"
//#include "RJDefs.h"
#include <atomic>


struct msg_param
{
	uv_write_t write;
	uv_buf_t buf;
};

class RJTcpClient
{
public:
	RJTcpClient();
	~RJTcpClient();

private:
	BufferReader m_reader;
	uv_loop_t* m_loop;
	uv_tcp_t m_uv_client;
	uv_connect_t m_connect;
	uv_async_t m_async_handle;
	std::atomic<bool> m_is_connected;
	std::atomic<bool> m_is_closing;

	std::mutex m_send_lock;
	std::queue<uv_buf_t> m_send_buf;

public:
	std::thread* m_p_thread;
	RJClientCallBack m_callback;

public:
	int Connect(const char* ip, int port);
	int Send(const char* msg, int size);
	void Close();

private:
	void OnConnection();
	void OnMsg(uv_tcp_t* client, const char* msg, int size);
	void OnError();
	void OnDisConnection();
	void RunThread();
	void CloseAll();

private:
	static void AllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void HandleConnection(uv_connect_t* connect, int status);
	static void HandleMsg(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void AsyncCallBack(uv_async_t* handle);
	static void AfterSend(uv_write_t* req, int status);
	static void DeleteWriteReq(uv_write_t* write);
	static void WalkCallBack(uv_handle_t* handle, void* arg);
};

