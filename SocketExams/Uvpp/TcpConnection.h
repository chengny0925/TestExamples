#pragma once
#include "uvpp/loop.hpp"
#include "uvpp/tcp.hpp"
#include <deque>
#include "uv.h"

#ifdef _WIN32
#include<Winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <string.h>
#endif // WIN

#define BufferReadCallBack std::function<void(TcpConnection*,const char*, int)>

#pragma pack(push,1)//将当前字节对齐值设为1
typedef struct
{
	unsigned char proto_type[2] = { 'u','v' };
	uint32_t size;
}PACKAGE_HEAD;
#pragma pack(pop)

#define PACKAGE_HEAD_SIZE sizeof(PACKAGE_HEAD)

class TcpConnection
{
public:
	/// type of callback for writing data
	typedef std::function<void(char const*, size_t)> do_write_t;

	/// when the protocol can't make sense of the data we should probably close the connection
	typedef std::function<void()> handle_error_t;


	TcpConnection(uvpp::loop &loop) :
		r_loop(loop)
		, m_tcp(loop)
		, m_write_in_progress(false)
	{
		m_index = 0;
		m_current_msg_size = 0;
		m_buffer_size = 1024 * 1024;
		m_msg_buffer = new char[m_buffer_size];
	};

	void set_peer_name(std::string& peer_name);

	void set_write_fun(do_write_t do_write)
	{
		m_do_write = do_write;
	}

	void input(const std::string& s)
	{
		input(s.c_str(), s.size());
	}
	/**
	* feed input data, for example from socket IO
	* Once a full message is read, handle_message is called
	*
	* to be called by the event library on read
	*/
	void input(const char* data, size_t len);


	/// to be called by the event library on write when the last write finished
	void on_write_finished();

	/**
	* will write the next output buffer by calling the write function @sa m_do_write
	* @post m_write_in_progress will be true
	*/
	void write_next_buff();

	void send_msg(const std::string&& msg);

	void set_msg_cb(BufferReadCallBack callback)
	{
		m_call_back = callback;
	}

private:
	void on_msg(const char* msg, int len);

private:
	/// internal input buffer accumulating data until it can be processed
	std::string m_input_buff;
	/// queue of buffers to write, we write from front to back, new appended to back, when wrote,
	/// removed from front.
	std::deque<std::string> m_output_buff;

	std::string m_peerName;

public:
	uvpp::loop &r_loop;
	uvpp::Tcp m_tcp;
	do_write_t m_do_write;

	bool m_write_in_progress;

	handle_error_t m_handle_error;

	BufferReadCallBack m_call_back;
	int m_current_msg_size;
	int m_index;
	int m_buffer_size;
	char *m_msg_buffer;
};

