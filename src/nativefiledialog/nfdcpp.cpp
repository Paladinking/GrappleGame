#include "nfdcpp.h"

nfd::result nfd::OpenDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath) {
	nfdchar_t* path;
	const char* fl = nullptr;
	const char* dp = nullptr;
	if (filterList != "") fl = filterList.c_str();
	if (defaultPath != "") dp = defaultPath.c_str();
	nfd::result r = NFD_OpenDialog(fl, dp, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}

nfd::result nfd::OpenDialog(std::string& outPath) {
	nfdchar_t* path;
	nfd::result r = NFD_OpenDialog(nullptr, nullptr, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}

nfd::result nfd::SaveDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath) {
	nfdchar_t* path;
	const char* fl = nullptr;
	const char*	dp = nullptr;
	if (filterList != "") fl = filterList.c_str();
	if (defaultPath != "") dp = defaultPath.c_str();
	nfd::result r = NFD_SaveDialog(fl, dp, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}

nfd::result nfd::SaveDialog(std::string& outPath) {
	nfdchar_t* path;
	nfd::result r = NFD_SaveDialog(nullptr, nullptr, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}

nfd::result nfd::PickFolder(const std::string& defaultPath, std::string& outPath) {
	nfdchar_t* path;
	const char* dp = defaultPath == "" ? nullptr : defaultPath.c_str();
	nfd::result r = NFD_PickFolder(dp, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}

nfd::result nfd::PickFolder(std::string& outPath) {
	nfdchar_t* path;
	nfd::result r = NFD_PickFolder(nullptr, &path);
	if (r == NFD_OKAY) {
		outPath = std::string(path);
		free(path);
	}
	return r;
}