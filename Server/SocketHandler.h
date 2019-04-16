#pragma once

#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

namespace Server{
	struct SocketData{
#ifdef WIN32
		SOCKET socket;
#endif
#if defined(__APPLE__)||defined(__linux__)
		int socket;
#endif
		bool log;
		int buffer_size;
		Application* master;
	};

	class SocketHandler{
	public:
		SocketHandler();
		~SocketHandler();
		void main(SocketData socket_data);
	};
}