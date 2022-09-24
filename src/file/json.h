#ifndef JSON_00_H
#define JSON_00_H
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>
#include "util/exceptions.h"

/**
 * json_exception, for when reading a json file fails.
 */
class json_exception : public base_exception {
	public:
		json_exception(std::string msg) : base_exception(msg) {};
};

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
		T& get(const std::string& key) {
			return std::get<T>(data[key]);
		}
		
		template<class T>
		const T& get(const std::string& key) const {
			return std::get<T>(data.at(key));
		}
		
		template<class T>
		const T &get_default(const std::string& key, const T& defalt_val) const {
			auto& el = data.find(key);
			if (el == data.end()) return defalt_val;
			const json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return defalt_val;
			}
			return std::get<T>(var);
		}
		
		template<class T>
		T& get_default(const std::string& key, T& defalt_val) {
			auto& el = data.find(key);
			if (el == data.end()) return defalt_val;
			json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return defalt_val;
			}
			return std::get<T>(var);
		}
		
		const json::Type& get(const std::string& key) const {
			return data.at(key);
		}
		
		json::Type& get(const std::string& key) {
			return data[key];
		}
		
		template<class T>
		void set(const std::string& key, const T& value) {
			if (!has_key(key)) keys.push_back(key);
			data[key] = value;
		}
		
		std::unordered_map<std::string, json::Type>::iterator begin() {
			return data.begin();
		}
		
		std::unordered_map<std::string, json::Type>::iterator end() {
			return data.end();
		}
		
		std::unordered_map<std::string, json::Type>::const_iterator begin() const {
			return data.begin();
		}
		
		std::unordered_map<std::string, json::Type>::const_iterator end() const {
			return data.end();
		}
		
		std::list<std::string>::const_iterator keys_begin() const {
			return keys.begin();
		}
		
		std::list<std::string>::const_iterator keys_end() const {
			return keys.end();
		}
		
		bool has_key(const std::string& key) const {
			return data.count(key) != 0;
		}
		
		template<class T>
		bool has_key_of_type(const std::string& key) const {
			auto& el = data.find(key);
			if (el == data.end()) return false;
			const json::Type& val = el->second;
			return std::get_if<T>(&val) != nullptr;
		}
		
		size_t size() const {
			return data.size();
		}
		
		void clear() {
			data.clear();
			keys.clear();
		}
		
		void to_pretty_stream(std::ostream& os, int indentations) const;
		
		void to_stream(std::ostream& os) const;
	
	private:
		std::unordered_map<std::string, json::Type> data;

		std::list<std::string> keys;
};

class JsonList {
	public:
		template<class T>
		T& get(const unsigned index) {
			return std::get<T>(data[index]);
		}
		
		template<class T>
		const T& get(const unsigned index) const {
			return std::get<T>(data[index]);
		}
		
		json::Type& get(const unsigned index) {
			return data[index];
		}
		
		const json::Type& get(const unsigned index) const {
			return data[index];
		}
		
		template<class T>
		void set(const unsigned index, const T& value) {
			data[index] = value;
		}
		
		template<class T>
		void push_back(const T& value) {
			data.emplace_back(value);
		}
		
		std::vector<json::Type>::iterator begin() {
			return data.begin();
		}
		
		std::vector<json::Type>::iterator end() {
			return data.end();
		}
		
		size_t size() const {
			return data.size();
		}
		
		void clear() {
			data.clear();
		}
		
		void to_pretty_stream(std::ostream& os, int indentations) const;
		
		void to_stream(std::ostream& os) const;
	
	private:
		std::vector<json::Type> data;
		
};

std::ostream& operator<<(std::ostream& os, const JsonObject &obj);

std::ostream& operator<<(std::ostream& os, const JsonList &list);

#endif