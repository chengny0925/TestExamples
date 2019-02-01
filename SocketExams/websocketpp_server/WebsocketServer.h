#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <string>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef connection_hdl WSHandle;

class IWebsocketServerCallBack
{
public:
	virtual void NewConnection(WSHandle wsi) = 0; //  新客户连接成功回调
	virtual void Disconnection(WSHandle wsi) = 0;   //  连接断开回调， 客户端主动断开或服务器端主动断开都会回调
	virtual void TextReceived(WSHandle wsi, const char *message, int len) = 0;   //  文本数据
	virtual void BinaryReceived(WSHandle wsi, const char *message, int len) = 0;  // 二进制数据
};

class IWebsocketServer
{
public:
	virtual int Init(int port) = 0;
	virtual void SetCallBack(IWebsocketServerCallBack* pCallBack) = 0;
	virtual int Start() = 0;                                               //  启动服务
	virtual void Stop() = 0;                                              //  结束服务

	virtual void CloseConnection(WSHandle wsi) = 0;                       //  主动关闭连接
	virtual int  SendText(WSHandle wsi, const char *message, int len) = 0;     //  发送文本
	virtual int  SendBinary(WSHandle wsi, const char *message, int len) = 0;   //  发二进制数据 
};

class WebsocketServer : public IWebsocketServer
{ 
public:
	WebsocketServer();
	~WebsocketServer();

	virtual int Init(int port) override;
	virtual void SetCallBack(IWebsocketServerCallBack* pCallBack) override;
	virtual int Start() override;
	virtual void Stop() override;
	virtual void CloseConnection(WSHandle wsi);
	virtual int  SendText(WSHandle wsi, const char *message, int len);
	virtual int  SendBinary(WSHandle wsi, const char *message, int len);

private:
	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_message(connection_hdl hdl, server::message_ptr msg);
	bool on_ping(connection_hdl hdl, std::string payload);

private:
	server m_server;
	int m_port;
	IWebsocketServerCallBack* m_pCallBack;
};

