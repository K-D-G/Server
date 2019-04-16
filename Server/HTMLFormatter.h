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