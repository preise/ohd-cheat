#pragma once

class network
{
private:
	std::string url = "https://api.github.com/repos/preise/ohd-dll/releases/latest";

public:
	std::string dll_path = "";

public:
	bool download_file(const std::string& url, const std::wstring& file_path);
	bool get_dll();
	std::string get_hash();

	int get_use_count();
};

namespace json
{
	std::string get_value(const std::string& json, const std::string& key);
}