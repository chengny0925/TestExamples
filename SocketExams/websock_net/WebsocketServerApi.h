#pragma once
#include "websock_net_global.h"
#include <stdint.h>

typedef void *WSHandle;
enum ThreadPriority
{
	TimeCritical,   //  ������ȼ�
	Highest,
	AboveNormal,
	Normal,
	BelowNormal,
	Lowest,
	Idle              //  ������ȼ�
};

class IWebsocketServerCallBack
{
public:                                              
	virtual void NewConnection(WSHandle wsi) = 0; //  �¿ͻ����ӳɹ��ص�
	virtual void Disconnection(WSHandle wsi) = 0;   //  ���ӶϿ��ص��� �ͻ��������Ͽ���������������Ͽ�����ص�
	virtual void TextReceived(WSHandle wsi, const char *message, int len) = 0;   //  �ı�����
	virtual void BinaryReceived(WSHandle wsi, const char *message, int len) = 0;  // ����������
};

class IWebsocketServer
{
public:
	virtual int Init(int port, ThreadPriority prior = Normal, unsigned threadNum = 1) = 0;
	virtual void SetCallBack(IWebsocketServerCallBack* pCallBack) = 0;
	virtual int Start() = 0;                                               //  ��������
	virtual void Stop() = 0;                                              //  ��������

	virtual void CloseConnection(WSHandle wsi) = 0;                       //  �����ر�����
	virtual int  SendText(WSHandle wsi, const char *message, int len) = 0;     //  �����ı�
	virtual int  SendBinary(WSHandle wsi, const char *message, int len) = 0;   //  ������������ 
};


WEBSOCK_NET_FUN IWebsocketServer* CreateWebsocketServer();