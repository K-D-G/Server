#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

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
		HTMLFormatter(int number_of_cached_pages);
		~HTMLFormatter();

		std::string format(std::string path, std::unordered_map<std::string, std::string> data, bool cache);

		std::string get_cached_page(std::string path);
		bool remove_cached_page(std::string path);
	};
}