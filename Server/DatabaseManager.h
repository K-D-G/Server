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