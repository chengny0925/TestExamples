#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <set>

// uvpp
//#include "uvpp/loop.hpp"
//#include "uvpp/timer.hpp"
//#include "uvpp/async.hpp"
//#include "uvpp/work.hpp"
//#include "uvpp/tcp.hpp"
//#include "uvpp/idle.hpp"
//#include "uvpp/resolver.hpp"

#include <memory>
#include <limits>

#include "Client.h"

#ifndef _WIN32
#include<unistd.h>

#define Sleep(x) usleep(1000 * x)
#endif // !_WIN32

int main()
{
//	uvpp::loop loop;
//	//uvpp::Resolver resolver(loop);
//	//resolver.resolve("localhost", [](const uvpp::error& error, bool ip4, const std::string& addr)
//	//{
//	//	if (error)
//	//	{
//	//		std::cout << error.str() << std::endl;
//	//		return;
//	//	}
//	//	std::cout << (ip4 ? "IP4" : "IP6") << ": " << addr << std::endl;
//	//});
//
//
//#if 1
//	uvpp::Tcp tcp(loop);
//	if (!tcp.connect("127.0.0.1", 33013, [](uvpp::error e) {
//
//		std::cout << "connected: " << e.str() << std::endl;
//
//	}))
//		std::cout << "error connect\n";
//#endif
//
//	std::thread t([&loop]()
//	{
//		std::cout << "Thread started: " << std::this_thread::get_id() << std::endl;
//		try
//		{
//			if (!loop.run())
//			{
//				std::cout << "error run\n";
//			}
//			std::cout << "Quit from event loop" << std::endl;
//		}
//		catch (...)
//		{
//			std::cout << "exception\n";
//		}
//	});
//
//	t.join();

	Client client;
	client.Connect("127.0.0.1", 12345);
	while (true)
	{
		char c = getchar();
		if (c == 'q')
		{
			break;
		}
		else if(c == 's')
		{
			client.Send("sdsd", 4);
		}
		Sleep(1);
	}

	return 0;
}