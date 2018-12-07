#pragma once
#include <uvpp/loop.hpp>
#include <uvpp/tcp.hpp>
#include <uvpp/async.hpp>
#include <mutex>
#include <deque>
#include "TcpConnection.h"
#include <thread>

class Client
{
public:
	typedef std::function<void()> on_error_t;
	typedef std::function<void()> on_connect_t;

	Client();
	~Client();

public:
	int Connect(const char* ip, int port);
	void Send(const char* msg, int size);
	void Close();

private:
	void RunThread();
	void OnMsg(TcpConnection* client, const char* msg, int size);
	void OnClose();
	void OnError();

private:
	void on_tcp_connect(uvpp::error error);
	void on_async();

private:
	std::string m_ip;
	int m_port;
	bool m_running;
	uvpp::loop m_loop;
	uvpp::Tcp m_tcp_conn;
	std::unique_ptr<TcpConnection> m_tcpConnectionPtr;
	bool m_is_connected;
	bool m_is_closing;

	on_error_t m_on_error;
	on_connect_t m_on_connect;

	uvpp::Async m_async;
	std::mutex m_send_lock;
	std::deque<std::string> m_sendBuf;

	std::thread* m_p_thread;
};

