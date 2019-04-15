#include "ApplicationServer.h"

#ifdef WIN32	
#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace Server;

Application::Application():name(""), port(10), log(true), protocol(TCP){

}

Application::~Application(){

}

void Application::on_start(){

}

void Application::on_terminate(){

}

Response Application::on_request(Request request){

}

bool Application::start(std::string app_name, int port_number, int app_protocol, int buffer_size, bool log_requests){
	name=app_name;
	port=port_number;
	log=log_requests;

#ifdef WIN32	
	WSADATA wsa_data;
	int iresult;
	SOCKET listen_socket=INVALID_SOCKET;
	SOCKET client_socket=INVALID_SOCKET;

	addrinfo* result=NULL;
	addrinfo hints;

	int isend_result;
	iresult=WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if(iresult!=0){
		if(log){
			std::cout<<"Windows socket initialisation failed with error code"<<iresult<<std::endl;
		}
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=(protocol==TCP)?(IPPROTO_TCP):(IPPROTO_UDP);
	hints.ai_flags=AI_PASSIVE;

	iresult=getaddrinfo(NULL, (std::to_string(port)).c_str(), &hints, &result);
	if(iresult!=0){
		if(log){
			std::cout<<"Cannot resolve port number and IP address of this server.\nSpecific error code:"<<WSAGetLastError()<<std::endl;
		}
		WSACleanup();
		return false;
	}

	listen_socket=socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(listen_socket==INVALID_SOCKET){
		std::cout<<"Cannot create a socket to listen on the port.\nSpecific error"<<WSAGetLastError()<<std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	iresult=bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if(iresult==SOCKET_ERROR){
		std::cout<<"Cannot bind the socket.\nSpecific error:"<<WSAGetLastError()<<std::endl;
		freeaddrinfo(result);
		closesocket(listen_socket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	iresult=listen(listen_socket, SOMAXCONN);
	if(iresult==SOCKET_ERROR){
		std::cout<<"Cannot listen for connections.\nSpecific error:"<<WSAGetLastError()<<std::endl;
		closesocket(listen_socket);
		WSACleanup();
		return false;
	}

	//Excluding this as we are using threading to deal with more connections
	//closesocket(listen_socket);

	if(log){
		std::cout<<"Server started:"<<std::endl;
	}
	on_start();
	
	//Start the threading and accept sockets
	while(true){
		SocketData data={accept(listen_socket, NULL, NULL), log, buffer_size, this};
		threader.new_thread<SocketHandler, SocketData>(data);
	}

	closesocket(listen_socket);
	WSACleanup();
#endif
	return true;
}

void Application::terminate(){
	on_terminate();
	this->~Application();
	if(log){
		std::cout << "Server terminated:" << std::endl;
	}
}