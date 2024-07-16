#include "pch.h"
#include "network.h"

std::wstring string_to_wstring(const std::string& str)
{
    int len;
    int str_len = (int)str.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str_len, 0, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str_len, &wstr[0], len);
    return wstr;
}

bool network::download_file(const std::string& url, const std::wstring& file_path)
{
    HRESULT hr = URLDownloadToFileW(NULL, string_to_wstring(url).c_str(), file_path.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

bool network::get_dll()
{
    HINTERNET hSession = InternetOpen(L"GitHub API", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hSession)
    {
        MessageBoxA(NULL, "Failed to open internet session.", "Error", MB_ICONERROR);
        return false;
    }

    std::wstring wurl = string_to_wstring(url);
    HINTERNET hConnect = InternetOpenUrl(hSession, wurl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect)
    {
        MessageBoxA(NULL, "Failed to open URL.", "Error", MB_ICONERROR);
        InternetCloseHandle(hSession);
        return false;
    }

    std::string response;
    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0)
    {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);

    auto json = nlohmann::json::parse(response);
    std::string download_url;

    for (const auto& asset : json["assets"])
    {
        if (asset["name"].get<std::string>().find(".dll") != std::string::npos)
        {
            download_url = asset["browser_download_url"].get<std::string>();
            break;
        }
    }

    if (download_url.empty())
    {
        MessageBoxA(NULL, "No DLL file found in the latest release.", "Error", MB_ICONERROR);
        return false;
    }

    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
    std::filesystem::path dll_path = temp_dir / "ohd.dll";

    if (!download_file(download_url, dll_path.wstring()))
    {
        MessageBoxA(NULL, "Failed to download the DLL file.", "Error", MB_ICONERROR);
        return false;
    }

    this->dll_path = dll_path.string();

    return true;
}