#pragma once

class memory
{
private:
	std::wstring target = L"HarshDoorstop-Win64-Shipping.exe";
	
public:
	std::string dll_path = "ohd.dll";
	DWORD pid;

private:
	bool get_pid();
	bool inject_dll(std::string dll_path);

public:
	bool init();

	void delete_dll();
};