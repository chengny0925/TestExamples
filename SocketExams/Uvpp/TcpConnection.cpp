
#include "TcpConnection.h"
#include <iostream>
#include <string.h>

using namespace std;

void TcpConnection::set_peer_name(std::string &peer_name)
{
	m_peerName = peer_name;
}

/**
* Check if we have a full message then decode it and handle, otherwise wait for more data, same for
* payload.
*/
void TcpConnection::input(const char *data, size_t len)
{
	string buf;
	buf.resize(len);
	memcpy(&buf[0], data, len);
	buf.push_back('\0');
	cout << m_peerName << ":" << buf << " len:" << len << endl;

	m_input_buff.append(data, len);
	memcpy(m_msg_buffer + m_index, data, len);
	while (true)
	{
		if (m_input_buff.size() > PACKAGE_HEAD_SIZE)
		{
			int msg_size;
			memcpy(&msg_size, (void*)(m_input_buff[2]), sizeof(int));//指针前两个字节是认证码
			int m_current_msg_size = ntohl(msg_size); //net2local

			if (m_input_buff[0] != 'u' || m_input_buff[1] != 'v')
			{
				std::cout << "UNPACKAGE ERROR!!" << std::endl; ///TODO error dispose
				on_msg(nullptr, -1);
				break;
			}

			if (m_input_buff.size() >= PACKAGE_HEAD_SIZE + m_current_msg_size)
			{
				on_msg(m_msg_buffer + PACKAGE_HEAD_SIZE, m_current_msg_size);
				memcpy(m_msg_buffer, m_msg_buffer + PACKAGE_HEAD_SIZE + m_current_msg_size, m_index - PACKAGE_HEAD_SIZE - m_current_msg_size);
				m_index = m_index - PACKAGE_HEAD_SIZE - m_current_msg_size;
			}
			else
			{
				break;
			}
		}
	}
}

void TcpConnection::send_msg(const std::string &&msg)
{
	const bool do_write = m_output_buff.empty() == true;
	m_output_buff.emplace_back(move(msg));
	if (do_write)
		write_next_buff();
}

void TcpConnection::on_msg(const char * msg, int len)
{
	if (m_call_back != nullptr)
	{
		m_call_back(this, msg, len);
	}
}

void TcpConnection::write_next_buff()
{
	assert(!m_write_in_progress);
	assert(!m_output_buff.empty());
	const string &buf = m_output_buff.front();
	const char * buffer = "abcd";
	//m_do_write(buf.c_str(), buf.size());
	m_do_write(buffer, 4);
	m_write_in_progress = true;
}

void TcpConnection::on_write_finished()
{
	m_write_in_progress = false;
	assert(!m_output_buff.empty());
	m_output_buff.pop_front();
	if (!m_output_buff.empty())
		write_next_buff();
}