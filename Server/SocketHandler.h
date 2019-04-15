#pragma once

#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

namespace Server{
	struct SocketData {
#ifdef WIN32
		SOCKET socket;
#endif
		bool log;
		int buffer_size;
		Application* master;
	};

	struct Request {
		int body_length;
		std::string body;
		std::string source_ip;
	};

	struct Response {
		int body_length;
		std::string body;
		std::string destination_ip;
	};

	class SocketHandler{
	public:
		SocketHandler();
		~SocketHandler();
		void main(SocketData socket_data);
	};
}