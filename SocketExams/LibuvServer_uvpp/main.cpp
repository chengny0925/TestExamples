#include <thread>
#include <iostream>
#include <string>
#include "Server.h"

#ifndef _WIN32
#include<unistd.h>

#define Sleep(x) usleep(1000 * x)
#endif // !_WIN32



using namespace std;

int main()
{
    Server s;
	
    s.set_port(12345);
    s.start();

	while (true)
	{
		char c = getchar();
		if (c == 'q')
		{
			break;
		}
		else if (c == 's')
		{
			s.Broadcast("sdsd");
		}
		Sleep(1);
	}

    return 0;
}