#include "pch.h"
#include "memory.h"
#include "md5.h"

bool memory::init()
{
	if (!get_pid())
	{
		MessageBoxA(NULL, "Failed to get PID", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!inject_dll(dll_path))
	{
		MessageBoxA(NULL, "Failed to inject the dll", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	MessageBoxA(NULL, "Successfully injected", "Success", MB_OK | MB_ICONINFORMATION);
	return true;
}

bool memory::get_pid()
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return false;

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return false;
    }

    do {
        size_t convertedChars = 0;
        char convertedName[MAX_PATH];
        wcstombs_s(&convertedChars, convertedName, pe32.szExeFile, _TRUNCATE);
        if (wcscmp(pe32.szExeFile, target.c_str()) == 0)
        {
            pid = pe32.th32ProcessID;
            CloseHandle(hProcessSnap);
            return true;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return false;
}

bool memory::inject_dll(std::string dll_path)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
    {
        MessageBoxA(NULL, "Failed to open target process", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    LPVOID pDllPathRemote = VirtualAllocEx(hProcess, NULL, dll_path.size() + 1, MEM_COMMIT, PAGE_READWRITE);
    if (pDllPathRemote == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory in target process", "Error", MB_OK | MB_ICONERROR);
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pDllPathRemote, dll_path.c_str(), dll_path.size() + 1, NULL))
    {
        MessageBoxA(NULL, "Failed to write to target process memory", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, pDllPathRemote, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleA("Kernel32");
    if (hKernel32 == NULL)
    {
        MessageBoxA(NULL, "Failed to get handle to Kernel32", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, pDllPathRemote, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    FARPROC pLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
    if (pLoadLibraryA == NULL)
    {
        MessageBoxA(NULL, "Failed to get address of LoadLibraryA", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, pDllPathRemote, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, pDllPathRemote, 0, NULL);
    if (hThread == NULL)
    {
        MessageBoxA(NULL, "Failed to create remote thread", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, pDllPathRemote, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pDllPathRemote, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

void memory::delete_dll()
{
    if (!DeleteFileA(dll_path.c_str()))
    {
        if (!MoveFileExA(dll_path.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
        {
            MessageBoxA(NULL, "Failed to delete or schedule deletion of the DLL file.", "Error", MB_ICONERROR);
        }
        else
        {
            std::string message = "The DLL is in use and will be deleted on next reboot or by you (" + dll_path + ").";
            MessageBoxA(NULL, message.c_str(), "Cleanup", MB_ICONINFORMATION);
        }

    }
    else
    {
        MessageBoxA(NULL, "The dll has been removed.", "Cleanup", MB_ICONINFORMATION);
    }
}

std::string memory::get_hash()
{
	std::ifstream file(dll_path, std::ios::binary);
	if (!file.is_open())
	{
		return "Failed to open the file.";
	}

	std::string hash = md5(file);
	file.close();

	return hash;
}