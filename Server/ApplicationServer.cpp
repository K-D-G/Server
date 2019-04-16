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
	Response result;
	result.body="HTTP/1.1 200 OK\r\nContent-Length:109\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<!DOCTYPE html><html><head><title>Example website</title></head><body><h1>Request received</h1></body></html>";
	result.body_length=result.body.size();
	result.destination_ip=request.source_ip;
	return result;
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
	PassThroughStruct pass_through_data={listen_socket, log, buffer_size};
	Threader::new_thread<Application, PassThroughStruct>(this, pass_through_data);

	closesocket(listen_socket);
	WSACleanup();
#endif
//Remove redundant variables, add UDP capabilities
#if defined(__APPLE__)||defined(__linux__)
	int server_fd;
	sockaddr_in address;
	int opt=1;
	int address_length=sizeof(address);
	char buffer[buffer_size]={0};

	if((server_fd=socket(AF_INET, SOCK_STREAM, 0))==0){
		std::cout<<"Socket creation failure"<<std::endl;
		return false;
	}

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt))){
		std::cout<<"Socket creation failure"<<std::endl;
		return false;
	}

	address.sin_family=AF_INET;
	address.sin_addr.s_addr=INADDR_ANY;
	address.sin_port=htons(port);

	if(bind(server_fd, (sockaddr*)&address, sizeof(address))<0){
		std::cout<<"Cannot bind the socket"<<std::endl;
		return false;
	}

	if(listen(server_fd, 3)<0){
		std::cout<<"Cannot get the socket to listen"<<std::endl;
		return false;
	}

	if(log){
		std::cout<<"Server started:"<<std::endl;
	}
	on_start();

	PassThroughStruct pass_through_data={server_fd, address, address_length, log, buffer_size};
	Threader::new_thread<Application, PassThroughStruct>(this, pass_through_data);

#endif
	return true;
}

#ifdef WIN32
void Application::main(PassThroughStruct s){
	while(true){
		SocketHandler* handler=new SocketHandler();
		SocketData data={accept(s.listen_socket, NULL, NULL), s.log, s.buffer_size, this};
		Threader::new_thread<SocketHandler, SocketData>(handler, data);
	}
}
#endif

#if defined(__APPLE__)||defined(__linux__)
void Application::main(PassThroughStruct s){
	while(true){
		SocketHandler handler;
		SocketData data={accept(s.server_fd, (sockaddr*)&s.address, (socklen_t*)&s.address_length), s.log, s.buffer_size, this};
		Threader::new_thread<SocketHandler, SocketData>(handler, data);
	}
}
#endif

void Application::terminate(){
	on_terminate();
	this->~Application();
	if(log){
		std::cout<<"Server terminated:"<<std::endl;
	}
}

SocketHandler::SocketHandler(){

}

SocketHandler::~SocketHandler(){

}

void SocketHandler::main(SocketData socket_data){
#ifdef WIN32
	int iresult;
	int send_result;

	char* receive_buffer=new char[socket_data.buffer_size];
	if(socket_data.socket==INVALID_SOCKET){
		std::cout<<"Cannot accept the socket.\nSpecific error:"<<WSAGetLastError()<<std::endl;
		closesocket(socket_data.socket);
		return;
	}

	char* ip=(char*)"";

	sockaddr_in name={0};
	int name_len=sizeof(name);
	getpeername(socket_data.socket, (sockaddr*)& name, &name_len);
	inet_ntop(AF_INET, &(name.sin_addr), ip, INET_ADDRSTRLEN);

	do{
		//iresult is number of bytes received
		iresult=recv(socket_data.socket, receive_buffer, socket_data.buffer_size, 0);
		if(iresult>0){
			if(socket_data.log){
				std::cout<<"Request received:"<<receive_buffer<<" ("<<iresult<<" bytes)"<<std::endl;
			}
			Request r={socket_data.buffer_size, receive_buffer, ip};
			Response response=socket_data.master->on_request(r);

			send_result=send(socket_data.socket, response.body.c_str(), response.body_length, 0);
			if(send_result==SOCKET_ERROR){
				if(socket_data.log){
					std::cout<<"Failed to send response.\nSpecific error:"<<WSAGetLastError()<<std::endl;
				}
				closesocket(socket_data.socket);
				return;
			}
		}
		else if(iresult!=0){
			if(socket_data.log){
				std::cout<<"Failed to receive data.\nSpecific error:"<<WSAGetLastError()<<std::endl;
				closesocket(socket_data.socket);
				return;
			}
		}

	}while(iresult>0);

	iresult=shutdown(socket_data.socket, SD_SEND);
	if(iresult==SOCKET_ERROR){
		if(socket_data.log){
			std::cout<<"Error shutting down socket.\nSpecific error:"<<WSAGetLastError()<<std::endl;
		}
	}
	closesocket(socket_data.socket);
#endif
#if defined(__APPLE__)||defined(__linux__)
	if(socket_data.socket<0){
		if(socket_data.log){
			std::cout<<"Error accepting socket"<<std::endl;
		}
		return;
	}

	char buffer[socket_data.buffer_size];
	int val_read=read(socket_data.socket, buffer, socket_data.buffer_size);
	if(socket_data.log){
		std::cout<<"Request received:"<<receive_buffer<<" ("<<socket_data.buffer_size<<" bytes)"<<std::endl;
	}

	char* ip=new char[20];
	sockaddr_in addr;
	socklen_t addr_size=sizeof(sockaddr_in);
	int result=getpeername(socket_data.socket, (sockaddr*)&addr, &addr_size);
	strcpy(ip, inet_ntoa(addr.sin_addr));

	Request r={socket_data.buffer_size, receive_buffer, ip};
	Response response=socket_data.master->on_request(r);
	send(socket_data.socket, response.body.c_str(), response.body_length, 0);
#endif
}