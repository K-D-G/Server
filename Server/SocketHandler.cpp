#include "SocketHandler.h"

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace Server;

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

	char* ip;

	sockaddr_in name={0};
	int name_len=sizeof(name);
	getpeername(socket_data.socket, (sockaddr*)& name, &name_len);
	inet_ntop(AF_INET, &(name.sin_addr), ip, INET_ADDRSTRLEN);

	do{
		//iresult is number of bytes received
		iresult=recv(socket_data.socket, receive_buffer, socket_data.buffer_size, 0);
		if(iresult>0){
			if(socket_data.log){
				std::cout<<"Request received:"<<receive_buffer<<" (" <<iresult <<" bytes)"<<std::endl;
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
			if (socket_data.log){
				std::cout<<"Failed to receive data.\nSpecific error:"<<WSAGetLastError()<<std::endl;
				closesocket(socket_data.socket);
				return;
			}
		}

	} while(iresult>0);

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