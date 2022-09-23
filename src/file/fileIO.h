#ifndef FILE_READER_00_H
#define FILE_READER_00_H
#include <SDL.h>
#include <string>
#include <cstring>
#include "util/exceptions.h"

/**
 * file_exception, for when opening a file fails.
 */
class file_exception : public base_exception {
	public:
		file_exception(std::string msg) : base_exception(msg) {};
};


template<class T>
T swap_byteorder(T &t) {
	T new_t;
	char* new_data = reinterpret_cast<char*>(&new_t);
	char* old_data = reinterpret_cast<char*>(&t);
	for (int j = 0; j < sizeof(T); ++j) {
		new_data[j] = old_data[sizeof(T) - 1 - j];
	}
	return new_t;
}


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
		
		/**
		 * Reads the next byte of data. Returns false if no more could be read.
		 */
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
		
		/**
		 * Skips the next byte of data. False if no more data could be found.
		 */
		bool skip() {
			char c;
			return read_next(c);
		}
		
		
		/**
		 * Reads data from file into T. Returns false if a full T object could not be read.
		 * Assumes the data is written with the same byte-order as used native.
		 */
		template<class T>
		bool read_next(T &res) {
			char* res_data = reinterpret_cast<char*>(&res);
			for (int j = 0; j < sizeof(T); ++j) {
				if (!read_next(res_data[j])) return false;
			}
			return true;
		}
		
		/**
		 * Reads nr objects of Type T into res. Returns false if not all objects could be read.
		 * This will not use the buffer at all, it will be cleared. If reading an amount of data 
		 * smaller than 256 bytes (the buffer size) it could be better to iterate over read_next<T>().
		 */ 
		template<class T>
		bool read_many(T* res, int nr) {
			++index;
			long pos = SDL_RWtell(in) - max + index;
			SDL_RWseek(in, pos, RW_SEEK_SET);
			long read = SDL_RWread(in, res, sizeof(T), nr);
			index = -1;
			max = 0;
			return read == nr;
		}
		
		
		/**
		 * Reads a null-terminated string from the file, allocates memory for it and pointing s to it.
		 * Returns false if no null-ptr could be read. Will consume bytes even when 
		 * returning false, but all memory will be freed and s will remain unchanged.
		 */
		bool read_string(char* &s) {
			char c;
			if (!read_next(c)) return false; // Read string only looks forwards.
			int buf_len = 0;
			char* buf = new char[buf_len];
			char* null_pos;
			do {
				null_pos = (char*)memchr(buffer + index, '\0', max - index);
				if (null_pos == NULL) {
					char* tmp = new char[buf_len + max - index];
					memcpy(tmp, buf, buf_len);
					memcpy(tmp + buf_len, buffer + index, max - index);
					delete[] buf;
					buf = tmp;
					buf_len += (max - index);
					
					index = max - 1; 		// 
					if(!read_next(c)) { 	// Read new data into buffer.
						delete[] tmp;
						return false;
					}
				} else {
					int len = 1 + null_pos - (buffer + index);
					char* tmp = new char[buf_len + len];
					memcpy(tmp, buf, buf_len);
					memcpy(tmp + buf_len, buffer + index, len);
					delete[] buf;
					buf = tmp;
					index += len - 1; // Should never move outside buffer since a null-byte was found here.
				}
			} while (null_pos == NULL);
			
			s = buf;
			return true;
		}
		
		/**
		 * Reads the byte at the current position (the value previously given by read_next())
		 * Returns false if this is not a valid byte of data.
		 */
		bool read_cur(char &c) const {
			if (index == max || index == -1) {
				return false;
			}
			c = buffer[index];
			return true;
		}

		/**
		 * Reverses one step without reloading the buffer, allowing read_next() to 
		 * give the same value it previously did.
		 */
		void soft_back() {
			--index;
			if (index < -1) {
				index = -1;
			}
		}

		/**
		 * Stores this position for a future reset.
		 */
		void mark() {
			mark(mark_index);
		}

		/**
		 * Stores this position in pos for a future reset.
		 */
		void mark(long &pos) {
			pos = SDL_RWtell(in) - max + index;
		}

		/**
		 * Moves the file position to the last marked value.
		 * This will always lead to reloading the buffer.
		 */
		void reset() {
			reset(mark_index);
		}

		/**
		 * Moves the file position to pos, reloading the buffer.
		 */
		void reset(long &pos) {
			SDL_RWseek(in, pos, RW_SEEK_SET);
			index = 0;
			max = SDL_RWread(in, &buffer, sizeof(char), 256 * sizeof(char));
		}

		/**
		 * Gets the current row and column.
		 */
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
		
		bool write(const char *s, long len) {
			long written = SDL_RWwrite(out, s, 1, len + 1);
			return written == len + 1;
		}
		
		bool write(const char c) {
			return SDL_RWwrite(out, &c, 1, 1) == 1;	
		}
		
		bool write(const int i) {
			return SDL_RWwrite(out, &i, sizeof(i), 1) == 1;
		}
		
		template<class T>
		bool write(const T& t) {
			return SDL_RWwrite(out, &t, sizeof(T), 1) == 1;
		}

		template<class T>
		bool write_many(const T* t, const int count) {
			return SDL_RWwrite(out, t, sizeof(T), count) == count;
		}

	private:
		SDL_RWops *out;
};

#endif