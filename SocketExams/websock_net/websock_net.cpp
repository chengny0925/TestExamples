// websock_net.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include  <log4cxx/log4cxx.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/logmanager.h>
#include <fstream>
#include <stdexcept>

static const char *config_file = "websock_net_config.xml";

class WebsockInitLog
{
public:
	WebsockInitLog()
	{
		CheckConfigFile();

		log4cxx::xml::DOMConfigurator::configureAndWatch(config_file, 30);
	}

	~WebsockInitLog()
	{
		try 
		{
			log4cxx::LogManager::shutdown();
		}
		catch (...)
		{

		}
	}

private:
	void CheckConfigFile()
	{
		try
		{
			std::ifstream ifs;
			ifs.open(config_file);
			if (!ifs.is_open())
			{
				GenerateConfig();
			}
		}
		catch (...)
		{
			GenerateConfig();
		}
	}

	void GenerateConfig()
	{
		return;
	}
};

//static WebsockInitLog log_;