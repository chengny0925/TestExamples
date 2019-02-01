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
	virtual int Start();                                               //  ��������
	virtual void Stop();                                              //  ��������

	virtual void CloseConnection(WSHandle wsi);                       //  �����ر�����
	virtual int  SendText(WSHandle wsi, const char *message, int len);     //  �����ı�
	virtual int  SendBinary(WSHandle wsi, const char *message, int len);   //  ������������ 
	int  Ping(WSHandle wsi, const char *payload = 0, int len = 0);
public:
	virtual int BeforeAccept(const char *ip) { return 0; }    //  ����ǰ�Ĺ��ˣ�return==0��ʾ�������ӣ� return!=0��ʾ�ܽ�����
	virtual void NewConnection(WSHandle wsi, const char *ip, int port);//  �¿ͻ����ӳɹ��ص�
	virtual void Disconnection(WSHandle wsi, const char *ip, int port);    //  ���ӶϿ��ص��� �ͻ��������Ͽ���������������Ͽ�����ص�
	virtual void TextReceived(WSHandle wsi, const char *message, int len);   //  �ı�����
	virtual void BinaryReceived(WSHandle wsi, const char *message, int len);  // ����������
	virtual void TextFrameReceived(WSHandle wsi, const char *message, int len, bool isLast) {}  //  �ı�֡
	virtual void BinaryFrameReceived(WSHandle wsi, const char *message, int len, bool isLast) {}  // ������֡
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