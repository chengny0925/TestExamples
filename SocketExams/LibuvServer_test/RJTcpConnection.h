#pragma once
#include "LibUvHelper.h"

class RJTcpConnection
{
public:
	RJTcpConnection();
	~RJTcpConnection();

private:
	uv_tcp_t client;
	BufferReader reader;
};

