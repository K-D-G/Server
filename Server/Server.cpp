// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//Including the library and iostream to output to console
#include <iostream>
#include "ApplicationServer.h"

using namespace Server;

//Definition of the HTTP server
class ExampleHTTPServer:public Application{
public:
	void on_start(){
		return;
	}

	//Creates the response for the request 
	Response on_request(Request* request){
		Response result;
		result.body="HTTP/1.1 200 OK\r\nContent-Length:109\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<!DOCTYPE html><html><head><title>Example website</title></head><body><h1>Request received</h1></body></html>";
		result.body_length=result.body.size();
		result.destination_ip=request->source_ip;
		return result;
	}

	void on_terminate(){
		return;
	}
};

//Creates an instance of the server and allows for requests. Stops termination by using cin.get();
int main(){
	ExampleHTTPServer app;
	app.start("Example HTTP server", 80, TCP, 512, true);
	std::cin.get();
	app.terminate();
	return 0;
}
