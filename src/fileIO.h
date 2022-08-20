#ifndef FILE_READER_00_H
#define FILE_READER_00_H
#include <SDL.h>
#include <string>
#include "exceptions.h"

/*
#include <exception>
class file_exception : std::exception {
	public:
		file_exception(std::string msg) : msg(msg) {};
		
		const std::string msg;
};*/


class FileReader {
	public:
		FileReader(std::string file_name, bool binary) {
			in = SDL_RWFromFile(file_name.c_str(), binary ? "rb" : "r");
			if (in == NULL) {
				throw file_exception("File exception: " + std::string(SDL_GetError()));
			}
			index = -1;
			max = SDL_RWread(in, &buffer, sizeof(char), 256 * sizeof(char));
		}
		
		~FileReader() {
			SDL_RWclose(in);
		}
		
		bool read_next(char &c) {
			++index;
			if (index == max) {
				index = 0;
				max = SDL_RWread(in, &buffer, sizeof(char), 256 * sizeof(char));
				if (max == 0) return false;
			} 
			c = buffer[index];
			return true;
		}
		
		bool read_cur(char &c) const {
			if (index == max || index == -1) {
				return false;
			}
			c = buffer[index];
			return true;
		}
		
		void soft_back() {
			--index;
			if (index < -1) {
				index = -1;
			}
		}
		
		void mark() {
			mark(mark_index);
		}
		
		void mark(long &pos) {
			pos = SDL_RWtell(in) - max + index;
		}
		
		void reset() {
			reset(mark_index);
		}
		
		void reset(long &pos) {
			SDL_RWseek(in, pos, RW_SEEK_SET);
			index = -1;
			max = SDL_RWread(in, &buffer, sizeof(char), 256 * sizeof(char));
		}
		
		void get_position(int &row, int &col) {
			long temp_mark;
			mark(temp_mark);
			SDL_RWseek(in, 0, RW_SEEK_SET);
			char *buf = new char[temp_mark];
			long read_bytes = 0;
			while (read_bytes < temp_mark) {
				long r = SDL_RWread(in, buf + read_bytes, 1, temp_mark - read_bytes);
				if (r == 0) {
					throw file_exception("Could not read from file, " + std::string(SDL_GetError()));
				}
				read_bytes += r;
			}
			row = 1;
			col = 1;
			for (int i = 0; i < temp_mark; ++i) {
				col++; 
				if (buf[i] == '\n') {
					col = 1;
					row++;
				} else if (buf[i] == '\t') {
					col += 4;
				}
			} 
			
			delete[] buf;
			reset(temp_mark);
		}
		
	
	private:
		char buffer[256];
		SDL_RWops *in;
		
		long mark_index = 0;
		
		long index = 0;
		long max = 0;
};

class FileWriter {
	public:
		FileWriter(std::string file_name, bool binary) {
			out = SDL_RWFromFile(file_name.c_str(), binary ? "wb" : "w");
			if (out == NULL) {
				throw file_exception("Could not open file, " + std::string(SDL_GetError()));
			}
		}
		
		~FileWriter() {
			SDL_RWclose(out);
		}
		
		bool write(const std::string &s) {
			long written = SDL_RWwrite(out, s.c_str(), 1, s.length());
			return written == s.length();
		}
		
		bool write(const char c) {
			return SDL_RWwrite(out, &c, 1, 1) == 1;
			
		}
	
	private:
		SDL_RWops *out;
};


#endif