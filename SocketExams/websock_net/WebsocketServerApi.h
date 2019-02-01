#pragma once
#include "websock_net_global.h"
#include <stdint.h>

typedef void *WSHandle;
enum ThreadPriority
{
	TimeCritical,   //  最高优先级
	Highest,
	AboveNormal,
	Normal,
	BelowNormal,
	Lowest,
	Idle              //  最低优先级
};

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
	virtual int Init(int port, ThreadPriority prior = Normal, unsigned threadNum = 1) = 0;
	virtual void SetCallBack(IWebsocketServerCallBack* pCallBack) = 0;
	virtual int Start() = 0;                                               //  启动服务
	virtual void Stop() = 0;                                              //  结束服务

	virtual void CloseConnection(WSHandle wsi) = 0;                       //  主动关闭连接
	virtual int  SendText(WSHandle wsi, const char *message, int len) = 0;     //  发送文本
	virtual int  SendBinary(WSHandle wsi, const char *message, int len) = 0;   //  发二进制数据 
};


WEBSOCK_NET_FUN IWebsocketServer* CreateWebsocketServer();