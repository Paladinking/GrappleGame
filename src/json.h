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

	/**
	 * Reads a JsonObject from a file.
	 */
	JsonObject read_from_file(std::string path);

	/**
	 * Writes a JsonObject to a file, in prettified form with indentations and newlines.
	 */
	void write_to_file(std::string path, const JsonObject &obj);

	/**
	 * Writes a JsonObject to a file.
	 * If pretty is true, indentations and newlines will be written.
	 */
	void write_to_file(std::string path, const JsonObject &obj, bool pretty);
	
	/**
	 *  Writes a json::Type variant to a stream, in prettified form with indentations and newlines.
	 */
	void to_pretty_stream(std::ostream& os, const Type& type);

	/**
	 *  Writes a json::Type variant to a stream, in compact form.
	 */
	void to_stream(std::ostream& os, const Type& type);
	
	/**
	 * Writes a string to a stream surrounded with quotes and replaces \, ", LF, tab, backspace, form feed and CR with
	 * \\, \", \n, \t, \b, \f  and \r respectively.
	 */
	void escape_string_to_stream(std::ostream& os, const std::string& s);
}


class JsonObject {
	public:
		template<class T>
		T &get(const std::string key) {
			return std::get<T>(data[key]);
		}
		
		template<class T>
		const T &get_default(const std::string key, const T &defalt_val) {
			auto& el = data.find(key);
			if (el == data.end()) return defalt_val;
			json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return defalt_val;
			}
			return std::get<T>(var);
		}
		
		template<class T>
		T &get_default(const std::string key, T &defalt_val) {
			auto& el = data.find(key);
			if (el == data.end()) return defalt_val;
			json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return defalt_val;
			}
			return std::get<T>(var);
		}
		
		json::Type &get(const std::string key) {
			return data[key];
		}
		
		template<class T>
		void set(const std::string key, const T value) {
			data[key] = value;
		}
		
		std::unordered_map<std::string, json::Type>::iterator begin() {
			return data.begin();
		}
		
		std::unordered_map<std::string, json::Type>::iterator end() {
			return data.end();
		}
		
		bool has_key(const std::string key) {
			return data.count(key) != 0;
		}
		
		template<class T>
		bool has_key_of_type(const std::string key) {
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
		T &get(const unsigned index) {
			return std::get<T>(data[index]);
		}
		
		json::Type &get(const unsigned index) {
			return data[index];
		}
		
		template<class T>
		void set(const unsigned index, T value) {
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