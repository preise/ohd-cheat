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
};

