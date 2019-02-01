#pragma once
#include "WebsocketServer.h"
#include <thread>
#include <set>
#include <atomic>
#include <vector>
#include <map>

using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

struct ConnectionMsg
{
	WSHandle wsi;
	std::string msg;

	ConnectionMsg(WSHandle w, std::string& m)
		: wsi(w), msg(m) {}
};

class Myserver : IWebsocketServerCallBack
{
public:
	Myserver();
	~Myserver();

	int Init(int port);
	int Start();                                               //  启动服务
	void Stop();

	virtual void NewConnection(WSHandle wsi) override; //  新客户连接成功回调
	virtual void Disconnection(WSHandle wsi) override;   //  连接断开回调， 客户端主动断开或服务器端主动断开都会回调
	virtual void TextReceived(WSHandle wsi, const char *message, int len) override;   //  文本数据
	virtual void BinaryReceived(WSHandle wsi, const char *message, int len) override;  // 二进制数据

private:
	void process_messages();
	void handle_msg(ConnectionMsg& msg);

private:
	IWebsocketServer* m_pServer;
	std::thread* m_pThread;
	std::atomic_bool m_running;

	mutex m_action_lock;
	condition_variable m_action_cond;
	std::queue<ConnectionMsg> m_receiveMsgVec;




	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;
	con_list m_connections;
	std::map<WSHandle, int, std::owner_less<connection_hdl>> m_con_map;
	mutex m_connection_lock;
	std::atomic_int m_con_id;
};