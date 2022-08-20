#ifndef JSON_00_H
#define JSON_00_H
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>

class JsonObject;

class JsonList;

class JsonNull {};

namespace json {
	enum JsonTypes {
		OBJECT = 0, LIST = 1, INT = 2, DOUBLE = 3, BOOL = 4, STRING = 5, NULL_VALUE = 6
	};
	
	typedef std::variant<JsonObject, JsonList, int, double, bool, std::string, JsonNull> Type;
	
	JsonObject read_from_file(std::string path);
	
	void write_to_file(std::string path, const JsonObject &obj);
	
	void write_to_file(std::string path, const JsonObject &obj, bool pretty);
	
	void to_pretty_stream(std::ostream& os, const Type& type);

	void to_stream(std::ostream& os, const Type& type);
}


class JsonObject {
	public:
		template<class T>
		T &get(std::string key) {
			return std::get<T>(data[key]);
		}
		
		json::Type &get(std::string key) {
			return data[key];
		}
		
		template<class T>
		void set(std::string key, T value) {
			data[key] = value;
		}
		
		std::unordered_map<std::string, json::Type>::iterator begin() {
			return data.begin();
		}
		
		std::unordered_map<std::string, json::Type>::iterator end() {
			return data.end();
		}
		
		bool has_key(std::string key) {
			return data.count(key) != 0;
		}
		
		template<class T>
		bool has_key_of_type(std::string key) {
			if (!has_key(key)) return false;
			json::Type &val = data[key];
			return std::get_if<T>(&val) != nullptr;
		}
		
		size_t get_size() const {
			return data.size();
		}
		
		void to_pretty_stream(std::ostream& os, int indentations) const;
		
		void to_stream(std::ostream& os) const;
	
	private:
		std::unordered_map<std::string, json::Type> data;
};

class JsonList {
	public:
		template<class T>
		T &get(unsigned index) {
			return std::get<T>(data[index]);
		}
		
		json::Type &get(unsigned index) {
			return data[index];
		}
		
		template<class T>
		void set(unsigned index, T value) {
			data[i] = value;
		}
		
		template<class T>
		void push_back(T value) {
			json::Type v = value;
			data.push_back(v);
		}
		
		std::vector<json::Type>::iterator begin() {
			return data.begin();
		}
		
		std::vector<json::Type>::iterator end() {
			return data.end();
		}
		
		size_t get_size() const {
			return data.size();
		}
		
		void to_pretty_stream(std::ostream& os, int indentations) const;
		
		void to_stream(std::ostream& os) const;
	
	private:
		std::vector<json::Type> data;
		
};

std::ostream& operator<<(std::ostream& os, const JsonObject &obj);

std::ostream& operator<<(std::ostream& os, const JsonList &list);

#endif