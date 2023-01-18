#ifndef JSON_00_H
#define JSON_00_H
#include <utility>
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
		explicit json_exception(std::string msg) : base_exception(std::move(msg)) {};
};

class JsonObject;

class JsonList;

class JsonNull {};

namespace json {

	typedef std::variant<JsonObject, JsonList, int, double, bool, std::string, JsonNull> Type;

	/**
	 * Reads a JsonObject from a file.
	 */
	JsonObject read_from_file(const std::string& path);

	/**
	 * Writes a JsonObject to a file, in prettified form with indentations and newlines.
	 */
	void write_to_file(std::string path, const JsonObject &obj);

	/**
	 * Writes a JsonObject to a file.
	 * If pretty is true, indentations and newlines will be written.
	 */
	void write_to_file(const std::string& path, const JsonObject &obj, bool pretty);
	
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

/**
 * Class for representing a Json object.
 */
class JsonObject {
	public:

		/**
		 * Gets a value of type T with key key from the object.
		 */
		template<class T>
		T& get(const std::string& key) {
			return std::get<T>(data[key]);
		}
		template<class T>
		const T& get(const std::string& key) const {
			return std::get<T>(data.at(key));
		}

		/**
		 * Gets a value of type T with key key from the object.
		 * If no such value exists, default_val is returned.
		 */
		template<class T>
		const T &get_default(const std::string& key, const T& default_val) const {
			const auto& el = data.find(key);
			if (el == data.end()) return default_val;
			const json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return default_val;
			}
			return std::get<T>(var);
		}
		template<class T>
		T& get_default(const std::string& key, T& default_val) {
			const auto& el = data.find(key);
			if (el == data.end()) return default_val;
			json::Type &var = el->second;
			if (!std::holds_alternative<T>(var)) {
				return default_val;
			}
			return std::get<T>(var);
		}

		/**
		 * Gets a json::Type with key key from the object.
		 */
		[[nodiscard]] const json::Type& get(const std::string& key) const {
			return data.at(key);
		}
		json::Type& get(const std::string& key) {
			return data[key];
		}

		/**
		 * Sets the value at key to value.
		 */
		template<class T>
		void set(const std::string& key, const T& value) {
			if (!has_key(key)) keys.push_back(key);
			data[key] = value;
		}

		/**
		 * Gets a beginning iterator to the underlying map. No iteration order is guaranteed.
		 */
		std::unordered_map<std::string, json::Type>::iterator begin() {
			return data.begin();
		}
		[[nodiscard]] std::unordered_map<std::string, json::Type>::const_iterator begin() const {
			return data.begin();
		}

		/**
		 * Gets an end iterator to the underlying map.
		 */
		std::unordered_map<std::string, json::Type>::iterator end() {
			return data.end();
		}
		[[nodiscard]] std::unordered_map<std::string, json::Type>::const_iterator end() const {
			return data.end();
		}

		/**
		 * Gets an beginning iterator to the keys of this object.
		 * The order of iteration is the order of insertion.
		 */
		[[nodiscard]] std::list<std::string>::const_iterator keys_begin() const {
			return keys.begin();
		}

		/**
		 * Gets an end iterator to the keys of this object.
		 */
		[[nodiscard]] std::list<std::string>::const_iterator keys_end() const {
			return keys.end();
		}

		/**
		 * Returns true if this object contains the key key.
		 */
		[[nodiscard]] bool has_key(const std::string& key) const {
			return data.count(key) != 0;
		}

		/**
		 * Returns true if this object contains the key key,
		 *	and the value at key has the type T.
		 */
		template<class T>
		[[nodiscard]] bool has_key_of_type(const std::string& key) const {
			const auto& el = data.find(key);
			if (el == data.end()) return false;
			const json::Type& val = el->second;
			return std::get_if<T>(&val) != nullptr;
		}

		/**
		 * Returns the number of elements in this object.
		 */
		[[nodiscard]] size_t size() const {
			return data.size();
		}

		/**
		 * Removes all elements from this object.
		 */
		void clear() {
			data.clear();
			keys.clear();
		}

		/**
		 * Outputs this object as text to a stream, using indentations and spaces.
		 * The keys will be ordered the same as the order of insertion.
		 */
		void to_pretty_stream(std::ostream& os, int indentations) const;

		/**
		 * Outputs this object as text to a stream.
		 * The keys will be ordered the same as the order of insertion.
		 */
		void to_stream(std::ostream& os) const;
	
	private:
		std::unordered_map<std::string, json::Type> data;

		std::list<std::string> keys;
};

/**
 * Class representing a Json list.
 */
class JsonList {
	public:

		/**
		 * Returns true if this list has an entry at index with type T.
		 */
		template<class T>
		[[nodiscard]] bool has_index_of_type(const unsigned index) const {
			if (index >= data.size()) return false;
			const json::Type& t = data[index];
			return std::get_if<T>(&t) != nullptr;
		}

		/**
		 * Gets the element of type T at index from this list.
		 */
		template<class T>
		T& get(const unsigned index) {
			return std::get<T>(data[index]);
		}
		template<class T>
		const T& get(const unsigned index) const {
			return std::get<T>(data[index]);
		}

		/**
		 * Gets the json::Type at index from this list.
		 */
		json::Type& get(const unsigned index) {
			return data[index];
		}
		[[nodiscard]] const json::Type& get(const unsigned index) const {
			return data[index];
		}

		/**
		 * Sets the entry at index to value.
		 */
		template<class T>
		void set(const unsigned index, const T& value) {
			data[index] = value;
		}

		/**
		 * Append an element of type T to the end of the list.
		 */
		template<class T>
		void push_back(const T& value) {
			data.emplace_back(value);
		}

		/**
		 * Gets an iterator to the beginning of the list.
		 */
		std::vector<json::Type>::iterator begin() {
			return data.begin();
		}
		[[nodiscard]] std::vector<json::Type>::const_iterator begin() const {
			return data.begin();
		}

		/**
		 * Gets an iterator to the end of the list.
		 */
		std::vector<json::Type>::iterator end() {
			return data.end();
		}
		[[nodiscard]] std::vector<json::Type>::const_iterator end() const {
			return data.end();
		}

		/**
		 * Returns the number of entries in the list.
		 */
		[[nodiscard]] size_t size() const {
			return data.size();
		}

		/**
		 * Removes all entries from the list.
		 */
		void clear() {
			data.clear();
		}

		/**
		 * Outputs this list as a string to a stream, using indentations and spaces.
		 */
		void to_pretty_stream(std::ostream& os, int indentations) const;

		/**
		 * Outputs this list as a string to a stream.
		 */
		void to_stream(std::ostream& os) const;
	
	private:
		std::vector<json::Type> data;
		
};
/**
 * Calls to_pretty_stream on obj.
 */
std::ostream& operator<<(std::ostream& os, const JsonObject &obj);

/**
 * Calls to_pretty_stream on list.
 */
std::ostream& operator<<(std::ostream& os, const JsonList &list);

#endif