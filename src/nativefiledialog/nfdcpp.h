#ifndef _NFDCPP_H
#define _NFDCPP_H
#include "nfd.h"
#include <stdlib.h>
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