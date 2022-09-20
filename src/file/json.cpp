#include <limits>
#include <cmath>
#include <sstream>
#include <cerrno>
#include "json.h"
#include "fileIO.h"
#include "util/exceptions.h"



json_exception expected_char(char c, FileReader &in) {
	int row, col;
	in.get_position(row, col); \
	return json_exception("Excpected a '" + std::string(1, c) + "' at row " + std::to_string(row) + " , col " + std::to_string(col));
}

json_exception unexpected_char(char c, FileReader &in) {
	int row, col;
	in.get_position(row, col);
	return json_exception("Unexpected character '" + std::string(1, c) + "' at row " + std::to_string(row) + " , col " + std::to_string(col));
}

json_exception to_big_number(FileReader &in) {
	int row, col;
	in.get_position(row, col); \
	return json_exception("To big number at row " + std::to_string(row) + " , col " + std::to_string(col));
}

json_exception end_of_file() {
	return json_exception("Unexpected end of file");
}

void skip_spacing(FileReader &in);

void validate_char(FileReader &in, char c);

JsonObject read_object(FileReader &in);

JsonList read_list(FileReader &in);

void read_matching(FileReader &in, std::string s);

bool read_number(FileReader &in, int &i_val, double &d_val);

void to_pretty_stream(std::ostream& os, const json::Type& val, int indentations);

std::string read_string(FileReader &in);

void read_matching(FileReader &in, std::string s) {
	char c;
	if (!in.read_cur(c)) throw end_of_file();
	for (const char c1 : s) {
		if (c != c1) throw unexpected_char(c, in);
		if (!in.read_next(c)) throw end_of_file();
	}
	in.soft_back();
	
}

void validate_char(FileReader &in, char c) {
	char c1;
	if (!in.read_cur(c1)) throw end_of_file();
	if (c != c1) throw expected_char(c, in);
}


bool read_number(FileReader &in, int &i_val, double &d_val) {
	bool is_int = true;
	char c;
	bool negative = false;
	if (!in.read_cur(c)) throw end_of_file();
	if (c == '-') {
		negative = true;
		if (!in.read_next(c)) throw end_of_file();
		
	} 
	if (c == '0') {
		if (!in.read_next(c) || c != '.') {
			i_val = 0;
			in.soft_back();
			return true;
		}
		is_int = false;
		if(!in.read_next(c)) throw end_of_file();
	}
	if (c - '0' > 9 || c - '0' < 0) {
		throw unexpected_char(c, in);
	}
	i_val = 0;
	d_val = 0;
	double pos = 1;
	while (true) {
		if (c - '0' <= 9 && c - '0' >= 0) {
			if (is_int) {
				if (i_val > INT_MAX / 10) {
					throw to_big_number(in);
				} 
				i_val *= 10;
				i_val += (c - '0');
			} else {
				pos *= 10;
				d_val += (c - '0') / pos;
			}
		} else if (c == '.') {
			is_int = false;
			d_val = (double)i_val;
		} else if (c == 'E' || c == 'e') {
			if (!in.read_next(c)) throw end_of_file();
			bool negate_exp = false;
			if (c == '-') negate_exp = true;
			if (c == '+' || c == '-') {
				if(!in.read_next(c)) throw end_of_file();
			} 
			double exp = 0;
			int exp_pos = 1;
			// Number with exponent is considered double.
			if (c - '0' > 9 || c - '0' < 0) throw unexpected_char(c, in);
			while (c - '0' <= 9 && c - '0' >= 0) {
				exp *= exp_pos;
				exp += (c - '0');
				exp_pos *= 10;
				if (!in.read_next(c)) break;
			}
			if (negative) {
				d_val *= -1;
			}
			in.soft_back();
			errno = 0;
			double factor = std::pow(10.0, exp);
			if (errno == ERANGE) {
				throw to_big_number(in);
			}
			d_val = d_val * factor;
			return false;
		} else {
			in.soft_back();
			if (negative) {
				i_val *= -1;
				d_val *= -1;
			}
			return is_int;
		}
		if(!in.read_next(c)) c = '\0';
	}
	
}

JsonObject read_object(FileReader &in) {
	char c;
	validate_char(in, '{');
	JsonObject obj;
	skip_spacing(in);
	if (!in.read_cur(c)) throw end_of_file();
	if (c == '}') {
		return obj;
	}
	while (true) {
		std::string key = read_string(in);
		skip_spacing(in);
		validate_char(in, ':');
		skip_spacing(in);
		if (!in.read_cur(c)) throw end_of_file();
		switch (c) {
			case '"': {
				std::string val = read_string(in);
				obj.set(key, val);
				break;
			}
			case '{' : {
				JsonObject val = read_object(in);
				obj.set(key, val);
				break;
			}
			case '[' : {
				JsonList val = read_list(in);
				obj.set(key, val);
				break;
			}
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':				
			case '4':				
			case '5':				
			case '6':				
			case '7':				
			case '8':				
			case '9': {
				int i_val;
				double d_val;
				if (read_number(in, i_val, d_val)) {
					obj.set(key, i_val);
				} else {
					obj.set(key, d_val);
				}
				break;
			}
			case 'n': {
				read_matching(in, "null");
				obj.set(key, JsonNull());
				break;
			}
			case 't': {
				read_matching(in, "true");
				obj.set<bool>(key, true);
				break;
			}
			case 'f': {
				read_matching(in, "false");
				obj.set<bool>(key, false);
				break;
			}
			default:
				throw unexpected_char(c, in);
		}
		skip_spacing(in);
		if (!in.read_cur(c)) throw end_of_file();
		if (c == '}') return obj;
		if (c != ',') {
			throw unexpected_char(c, in);
		}
		skip_spacing(in);
	}
}

JsonList read_list(FileReader &in) {
	char c;
	validate_char(in, '[');
	JsonList list;
	skip_spacing(in);
	if (!in.read_cur(c)) throw end_of_file();
	if (c == ']') return list;
	while (true) {
		switch (c) {
			case '"': {
				std::string val = read_string(in);
				list.push_back(val);
				break;
			}
			case '{' : {
				JsonObject val = read_object(in);
				list.push_back(val);
				break;
			}
			case '[' : {
				JsonList val = read_list(in);
				list.push_back(val);
				break;
			}
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':				
			case '4':				
			case '5':				
			case '6':				
			case '7':				
			case '8':				
			case '9': {
				int i_val;
				double d_val;
				if (read_number(in, i_val, d_val)) {
					list.push_back(i_val);
				} else {
					list.push_back(d_val);
				}
				break;
			}
			case 'n': {
				read_matching(in, "null");
				list.push_back(JsonNull());
				break;
			}
			case 't': {
				read_matching(in, "true");
				list.push_back<bool>(true);
				
				break;
			}
			case 'f': {
				read_matching(in, "false");
				list.push_back<bool>(false);
				break;
			}
			default:
				throw unexpected_char(c, in);
		}
		skip_spacing(in);
		if (!in.read_cur(c)) throw end_of_file();
		if (c == ']') return list;
		else if (c != ',') {
			throw unexpected_char(c, in);
		}
		skip_spacing(in);
		if (!in.read_cur(c)) throw end_of_file();
	}
}

std::string read_string(FileReader &in) {
	char c;
	validate_char(in, '"');
	std::string res = ""; 
	while (in.read_next(c)) {
		if (c == '\\') {
			if (!in.read_next(c)) throw end_of_file();
			if (c == '\\' || c == '"' || c == '/') {
				res.push_back(c);
			} else if (c == 'n') {
				res.push_back('\n');
			} else if (c == 't') {
				res.push_back('\t');
			} else if (c == 'b') {
				res.push_back('\b');
			} else if (c == 'f') {
				res.push_back('\f');
			} else if (c == 'r') {
				res.push_back('\r');
			} else {
				throw json_exception("Unsuported escape character: \\" + std::string(1, c));
			}
		} else if (c == '"') {
			return res;
		} else {
			res.push_back(c);
		}
	}
	throw end_of_file();
} 


void skip_spacing(FileReader &in) {
	char c;
	while (in.read_next(c)) {
		if (c != ' ' && c != '\n' && c != '\r' && c != '\t') return;
	}
}

JsonObject json::read_from_file(std::string path) {
	FileReader reader(path, false);
	skip_spacing(reader);
	return read_object(reader);
}

void json::to_pretty_stream(std::ostream& os, const json::Type& val) {
	to_pretty_stream(os, val, 0);
}

void json::escape_string_to_stream(std::ostream& os, const std::string& s) {
	os << '"';
	for (char c : s) {
		switch (c) {
			case '\\':
				os << "\\\\";
				break;
			case '\"':
				os << "\\\"";
				break;
			case '\n':
				os << "\\n";
				break;
			case '\t':
				os << "\\t";
				break;
			case '\b':
				os << "\\b";
				break;
			case '\f':
				os << "\\f";
			case '\r':
				os << "\\r";
				break;
			default:
				os << c;
		}
	}
	os << '"';
}

void json::to_stream(std::ostream& os, const json::Type& val) {
	if (const JsonObject *obj = std::get_if<JsonObject>(&val)) {
		obj->to_stream(os);
	} else if (const JsonList *list = std::get_if<JsonList>(&val)) {
		list->to_stream(os);
	} else if (const JsonNull *null_obj = std::get_if<JsonNull>(&val)) {
		os << "null";
	} else if (const int* i = std::get_if<int>(&val)) {
		os << *i;
	} else if (const double* d = std::get_if<double>(&val)) {
		os << *d;
	} else if (const bool* b = std::get_if<bool>(&val)) {
		if (*b) os << "true";
		else os << "false";
	} else if (const std::string *s = std::get_if<std::string>(&val)) {
		json::escape_string_to_stream(os, *s);
	}
}


void json::write_to_file(std::string path, const JsonObject &obj) {
	write_to_file(path, obj, true);
}

void json::write_to_file(std::string path, const JsonObject &obj, bool pretty) {
	FileWriter writer(path, false);
	std::stringstream s;
	if (pretty) {
		s << obj;
	} else {
		obj.to_stream(s);
	}
	std::string str = s.str();
	writer.write(str);
	
}

void to_pretty_stream(std::ostream& os, const json::Type& val, int indentations) {
	if (const JsonObject *obj = std::get_if<JsonObject>(&val)) {
		obj->to_pretty_stream(os, indentations);
	} else if (const JsonList *list = std::get_if<JsonList>(&val)) {
		list->to_pretty_stream(os, indentations);
	} else if (const JsonNull *null_obj = std::get_if<JsonNull>(&val)) {
		os << "null";
	} else if (const int* i = std::get_if<int>(&val)) {
		os << *i;
	} else if (const double* d = std::get_if<double>(&val)) {
		os << *d;
	} else if (const bool* b = std::get_if<bool>(&val)) {
		if (*b) os << "true";
		else os << "false";
	} else if (const std::string *s = std::get_if<std::string>(&val)) {
		json::escape_string_to_stream(os, *s);
	}
}

void JsonObject::to_pretty_stream(std::ostream& os, int indentations) const {
	os << '{';
	if (data.size() == 0) {
		os << '}';
		return;
	}
	os << '\n';
	
	for (auto &it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin()) os << ",\n";
		for (int i = 0; i < indentations + 1; i++) {
			os << '\t';
		}
		os << "\"" << it->first << "\" : ";
		::to_pretty_stream(os, it->second, indentations + 1);
	}
	os << '\n';
	for (int i = 0; i < indentations; i++) os << '\t';
	os << '}';
}

void JsonObject::to_stream(std::ostream& os) const {
	os << '{';
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin()) os << ',';
		os << '"' << it->first << "\":";
		json::to_stream(os, it->second);
	}
	os << '}';
}

void JsonList::to_pretty_stream(std::ostream& os, int indentations) const {
	os << '[';
	if (data.size() == 0) {
		os << ']';
		return;
	}
	os << '\n';
	
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin()) os << ",\n";
		for (int i = 0; i < indentations + 1; i++) {
			os << '\t';
		}
		const json::Type &val = *it;
		::to_pretty_stream(os, val, indentations + 1);
	}
	os << '\n';
	for (int i = 0; i < indentations; i++) os << '\t';
	os << ']';
}

void JsonList::to_stream(std::ostream& os) const {
	os << '[';
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin()) os << ',';
		const json::Type &val = *it;
		json::to_stream(os, val);
	}
	os << ']';
}

std::ostream& operator<<(std::ostream& os, const JsonObject &obj) {
	obj.to_pretty_stream(os, 0);
	return os;
}

std::ostream& operator<<(std::ostream& os, const JsonList &list) {
	list.to_pretty_stream(os, 0);
	return os;
}