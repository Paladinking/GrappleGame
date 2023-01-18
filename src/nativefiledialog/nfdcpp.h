#ifndef NFD_CPP_00_H
#define NFD_CPP_00_H
#include "nfd.h"
#include <utility>
#include <string>
#include <vector>

namespace nfd {
	typedef nfdresult_t result;

	result OpenDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);

	result OpenDialog(std::string& outPath);

	result SaveDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);
	
	result SaveDialog(std::string& outPath);

	result PickFolder(const std::string& defaultPath, std::string& outPath);

	result PickFolder(std::string& outPath);
}
#endif