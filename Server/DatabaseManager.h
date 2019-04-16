#pragma once

#include <string>
#include <fstream>

namespace Server{
	struct DatabaseObject{
	
	};
	
	class DatabaseManager{
	public:
		DatabaseManager(std::string database_file_path);
		~DatabaseManager();

		DatabaseObject* query_table(std::string table_name, std::string query);
	};
}