#pragma once
#include <iostream>
#include <string>
#include "SocketHandler.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#if defined(__APPLE__)||defined(__linux__)

#endif

#define TCP 1
#define UDP 2
namespace Server{
	class Application {
	private:
		std::string name;
		int port;
		int protocol;
		bool log;
	public:
		Application();
		~Application();

		bool start(std::string app_name, int port_number, int app_protocol, int buffer_length, bool log_requests);
		void terminate();

		//Override these methods
		void on_start();
		Response on_request(Request request);
		void on_terminate();
	};
}