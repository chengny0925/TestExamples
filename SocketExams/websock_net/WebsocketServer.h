#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "websock_net_global.h"
#include <stdint.h>
#include "WebsocketServerApi.h"

class  WebsocketServer : public IWebsocketServer
{
public:
	WebsocketServer();
	virtual ~WebsocketServer();

	virtual int Init(int port, ThreadPriority prior = Normal, unsigned threadNum = 1);
	virtual void SetCallBack(IWebsocketServerCallBack* pCallBack);
	virtual int Start();                                               //  启动服务
	virtual void Stop();                                              //  结束服务

	virtual void CloseConnection(WSHandle wsi);                       //  主动关闭连接
	virtual int  SendText(WSHandle wsi, const char *message, int len);     //  发送文本
	virtual int  SendBinary(WSHandle wsi, const char *message, int len);   //  发二进制数据 
	int  Ping(WSHandle wsi, const char *payload = 0, int len = 0);
public:
	virtual int BeforeAccept(const char *ip) { return 0; }    //  连接前的过滤，return==0表示接受连接， return!=0表示拒接连接
	virtual void NewConnection(WSHandle wsi, const char *ip, int port);//  新客户连接成功回调
	virtual void Disconnection(WSHandle wsi, const char *ip, int port);    //  连接断开回调， 客户端主动断开或服务器端主动断开都会回调
	virtual void TextReceived(WSHandle wsi, const char *message, int len);   //  文本数据
	virtual void BinaryReceived(WSHandle wsi, const char *message, int len);  // 二进制数据
	virtual void TextFrameReceived(WSHandle wsi, const char *message, int len, bool isLast) {}  //  文本帧
	virtual void BinaryFrameReceived(WSHandle wsi, const char *message, int len, bool isLast) {}  // 二进制帧
	virtual void Pong(WSHandle wsi, uint64_t elapsedMilliSecs,  const char *payload, int len) {}
	
	//////////////////////////////////////////////
	void *MetaData() const;
	
private:
	IWebsocketServerCallBack* m_callBack;
	void *meta_data_;
	
	int port_;
	unsigned thread_num_;
	ThreadPriority thread_priority_;
};


#endif  // WEBSOCKETSERVER_H