#pragma once
#include <iostream>
#include <string>
#include "Threader.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#if defined(__APPLE__)||defined(__linux__)
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#endif

#define TCP 1
#define UDP 2
namespace Server{
	struct PassThroughStruct{
#ifdef WIN32
		SOCKET listen_socket;
#endif
#if defined(__APPLE__)||defined(__linux__)
		int server_fd;
		sockaddr_in address;
		int address_length;
#endif
		bool log;
		int buffer_size;
	};

	struct Request{
		int body_length;
		std::string body;
		std::string source_ip;
	};

	struct Response{
		int body_length;
		std::string body;
		std::string destination_ip;
	};

	class Application{
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

		//Don't call this function it is just for the threader
		void main(PassThroughStruct s);
	};

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