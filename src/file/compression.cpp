#include <SDL.h>
#define ZLIB_CONST
#include <zconf.h>
#include <zlib.h>
#include <climits>

#define BUFFER_SIZE 65536

struct CompressData {
	z_stream stream;
	unsigned char buffer[BUFFER_SIZE];
	unsigned buffer_pos;
	int(*data_filter) (z_stream*, int);
	int(*end_filter) (z_stream*);
};

Sint64 compressSize(SDL_RWops*) {
	return -1;
}

Sint64 compressSeek(SDL_RWops*, Sint64, int) {
	return -1;
}

size_t compressRead(SDL_RWops* ptr, void* dest, size_t size, size_t num) {
	if (num == 0 || size == 0) {
		return 0;
	}
	
	const unsigned target = static_cast<unsigned>(size * num > UINT_MAX ?  (UINT_MAX / size) * size : size * num);

	unsigned read_bytes = 0;
	SDL_RWops* source = static_cast<SDL_RWops*>(ptr->hidden.unknown.data1);
	CompressData* comp_data = static_cast<CompressData*>(ptr->hidden.unknown.data2);
	z_stream* strm = &comp_data->stream;
	
	if(strm->next_out != nullptr) {
		// This stream is being used to write data
		return 0;
	}
	
	strm->avail_out =  target;
	strm->next_out = static_cast<unsigned char*>(dest);
	
	if (strm->next_in == nullptr) {
		strm->next_in = comp_data->buffer;
		strm->avail_in = 0;
	}

	while (read_bytes < target) {
		if (strm->avail_in == 0) {
			strm->next_in = comp_data->buffer;
			strm->avail_in = static_cast<unsigned>(SDL_RWread(source, comp_data->buffer, 1, BUFFER_SIZE));
		}
		const int flush = strm->avail_in == 0 ? Z_FINISH : Z_NO_FLUSH;
		int ret = comp_data->data_filter(strm, flush);

		if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR) {
			break;
		}
		read_bytes = target - strm->avail_out;

		if (ret == Z_STREAM_END || flush == Z_FINISH || strm->avail_out == 0) {
			break;
		}
	}
	strm->next_out = nullptr;
	strm->avail_out = 0;
	return read_bytes / size;
} 

size_t compressWrite(SDL_RWops* ptr, const void* data, size_t size, size_t num) {
	if (num == 0 || size == 0) {
		return 0;
	}

	const unsigned target = static_cast<unsigned>(size * num > UINT_MAX ? (UINT_MAX / size) * size : size * num);
	
	unsigned consumed_bytes = 0;
	SDL_RWops* source = static_cast<SDL_RWops*>(ptr->hidden.unknown.data1);
	CompressData* comp_data = static_cast<CompressData*>(ptr->hidden.unknown.data2);
	z_stream* strm = &comp_data->stream;
	if (strm->next_in != nullptr) {
		// This stream is being used to read data
		return 0;
	}

	strm->next_in = static_cast<const unsigned char*>(data);
	strm->avail_in = target;

	if (comp_data->buffer_pos > 0) {
		const unsigned have = BUFFER_SIZE - comp_data->buffer_pos - strm->avail_out;
		const unsigned out = static_cast<unsigned>(SDL_RWwrite(source, comp_data->buffer + comp_data->buffer_pos, 1, have));
		if (out < have) {
			comp_data->buffer_pos += out;
			return 0;
		}
		comp_data->buffer_pos = 0;
	}

	if (strm->next_out == nullptr) {
		strm->next_out = comp_data->buffer;
		strm->avail_out = BUFFER_SIZE;
	}
	
	while (true) {
		strm->next_out = comp_data->buffer;
		strm->avail_out = BUFFER_SIZE;
		int ret = comp_data->data_filter(strm, Z_FINISH);
		if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
			break;
		}

		consumed_bytes = target - strm->avail_in;
		const unsigned have = BUFFER_SIZE - strm->avail_out;
		const unsigned out = static_cast<unsigned>(SDL_RWwrite(source, comp_data->buffer, 1, have));
		if (out < have) {
			comp_data->buffer_pos = out;
			break;
		}
		if (ret == Z_STREAM_END || (ret == Z_BUF_ERROR && strm->avail_out == BUFFER_SIZE)) {
			break;
		}
	}
	strm->next_in = nullptr;
	strm->avail_in = 0;
	return consumed_bytes / size;
} 

int compressClose(SDL_RWops* ptr) {
	SDL_RWops* source = static_cast<SDL_RWops*>(ptr->hidden.unknown.data1);
	CompressData* comp_data = static_cast<CompressData*>(ptr->hidden.unknown.data2);
	z_stream* strm = &comp_data->stream;
	comp_data->end_filter(strm);

	int ret = source->close(source);
	delete comp_data;

	SDL_FreeRW(ptr);
	return ret;
}

SDL_RWops* SDL_RWcompress(SDL_RWops* source, bool def) {
	if (source == nullptr) 
		return nullptr;
	SDL_RWops* compressor = SDL_AllocRW();
	if (compressor == nullptr) 
		return nullptr;

	CompressData* comp_data = new CompressData();
	z_stream* zstream = &comp_data->stream;
	comp_data->buffer_pos = 0;

	zstream->zalloc = Z_NULL;
    zstream->zfree = Z_NULL;
    zstream->opaque = Z_NULL;
	int ret;
	if (def) {
		comp_data->data_filter = deflate;
		comp_data->end_filter = deflateEnd;
		ret = deflateInit(zstream, Z_DEFAULT_COMPRESSION);
	} else {
		comp_data->data_filter = inflate;
		comp_data->end_filter = inflateEnd;
		ret = inflateInit(zstream);
	}
	
	if (ret != Z_OK) {
		delete comp_data;
		SDL_FreeRW(compressor);
		return nullptr;
	}

	zstream->next_in = nullptr;
	zstream->next_out = nullptr;
	zstream->avail_in = 0;
	zstream->avail_out = 0;

	compressor->size = compressSize;
	compressor->seek = compressSeek;
	compressor->read = compressRead;
	compressor->write = compressWrite;
	compressor->close = compressClose;
	compressor->hidden.unknown = {source, comp_data};
	compressor->type = SDL_RWOPS_UNKNOWN;
	return compressor;
}

SDL_RWops* SDL_RWinflate(SDL_RWops* source) {
	return SDL_RWcompress(source, false);
}

SDL_RWops* SDL_RWdeflate(SDL_RWops* source) {
	return SDL_RWcompress(source, true);
}