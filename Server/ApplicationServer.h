#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <thread>
#include <vector>

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

namespace Server{
	/*
	class Threader{
	public:
		Threader();
		~Threader();

		template<class T, typename Data>
		static void new_thread(T* processor, Data data){
			std::thread temp_thread(processor->main, data);
			temp_thread.join();
		}
	};
	*/
	std::vector<std::thread> Threads;
}



/*
Templating:
This is a very simple templating engine so only has one function which is to replace variable names with the data.
You do this like so: [;VARIABLENAME;]
*/

namespace Server{
	class HTMLFormatter{
	private:
		int index;
		std::vector<std::string> cache_file_names;
		std::vector<std::string> cache_file_data;
	public:
		HTMLFormatter(int number_of_cached_pages){
			index=0;
			cache_file_names.reserve(number_of_cached_pages);
			cache_file_data.reserve(number_of_cached_pages);
		}
		~HTMLFormatter();

		std::string format(std::string path, std::unordered_map<std::string, std::string> data, bool cache){
			std::string result;

			std::string file_data;
			std::string line;
			std::ifstream file_object(path);
			if(file_object.is_open()){
				while(std::getline(file_object, line)){
					file_data.append(line);
				}
				file_object.close();
			}else{
				return "";
			}

			for(auto it=data.begin(); it!=data.end(); ++it){
				std::string::size_type n=0;
				std::string temp="[;";
				temp.append(it->first);
				temp.append(";]");
				while((n=file_data.find(temp, n))!=std::string::npos){
					file_data.replace(n, temp.size(), it->second);
					n+=it->second.size();
				}
			}

			if(cache){
				cache_file_names[index]=path;
				cache_file_data[index]=result;
				index++;
				if(index>=cache_file_names.capacity()){
					index=0;
				}
			}
			return result;

		}

		std::string get_cached_page(std::string path){
			for(int i=0; i<cache_file_names.size(); i++){
				if(cache_file_names[i]==path){
					return cache_file_data[i];
				}
			}
			return "";
		}
		bool remove_cached_page(std::string path){
			for(int i=0; i<cache_file_names.size(); i++){
				if(cache_file_names[i]==path){
					cache_file_names[i]="";
					cache_file_data[i]="";
					return true;
				}
			}
			return false;
		}
	};
}

/*

#pragma once

#include <string>
#include <fstream>

namespace Server{
	//100 bytes long
	struct DatabaseHeader{
		char header_string[16]; //"SQLite format 3\000" \000 means NULL termination
		unsigned short page_size; //In bytes
		unsigned char file_format_write_version; //1 for legacy, 2 for WAL
		unsigned char file_format_read_version; //Same as above
		char reserved;
		unsigned char maximum_embedded_payload_fraction; //Must be 64
		unsigned char minimum_embedded_payload_fraction; //Must be 32
		unsigned char leaf_payload_fraction; //Must be 32
		unsigned int file_change_counter;
		unsigned int size_of_database_file_in_pages;
		unsigned int page_number_first_freelist_trunk;
		unsigned int number_freelist_pages;
		unsigned int schema_cookie;
		unsigned int schema_format_number; //1, 2, 3 or 4
		unsigned int default_page_cache_size;
		unsigned int page_number_largest_root_b_tree_page; //when in auto-vacuum or incremental-vacuum modes, or zero otherwise.
		unsigned int text_encoding; //1=UTF-8 2=UTF-16le 3=UTF-16be
		unsigned int user_version; //Read and set by user version pragma
		unsigned int incremental_vacuum_mode; //True(non-zero) or False(zero)
		unsigned int application_id; //Set by PRAGMA application id
		char reserved_for_expansion[20];
		unsigned int sqlite_version_number;
	};

	//The data inside must not be touched it is used by the operating system
	//This struct exists as a place holder
	struct LockBytePage{
		char data[513]; //Might be 511 instead
	};

	//Says which pages are not in active use
	//These pages aren't really touched to avoid using disk I/O alot
	struct FreeListPage{
		//Index 0 is the index of the next free list page (0 if there are no more)
		//Index 1 (Called L) is the number of leaf page pointers to follow
		//Index 2-L+1 inclusive is the page numbers
		unsigned int* pages;
	};

	struct BTreePage{

	};

	struct DatabaseObject{

	};

	class DatabaseManager{
	public:
		DatabaseManager(std::string database_file_path){

		}
		~DatabaseManager(){

		}

		DatabaseObject* query_table(std::string table_name, std::string query){
			DatabaseObject* result={};
			return result;
		}
	};
}
*/



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