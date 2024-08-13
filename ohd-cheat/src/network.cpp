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

int network::get_use_count()
{
	int counter = 0;
    return counter;
}

std::string network::get_hash()
{
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        // WinHttpOpen failed
        return "";
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"preise.one",
        INTERNET_DEFAULT_HTTP_PORT, 0);

    if (!hConnect)
    {
        // WinHttpConnect failed
        WinHttpCloseHandle(hSession);
        return "";
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/ohd/latest-release",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (!hRequest)
    {
        // WinHttpOpenRequest failed
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    if (!bResults)
    {
        // WinHttpSendRequest failed
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults)
    {
        // WinHttpReceiveResponse failed
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    DWORD dwSize = 0;
    std::string response;
    std::string result;
    do {
        DWORD dwDownloaded = 0;
        if (WinHttpQueryDataAvailable(hRequest, &dwSize))
        {
            char* buffer = new char[dwSize + 1];
            ZeroMemory(buffer, dwSize + 1);
            if (WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded))
            {
                response.append(buffer, dwDownloaded);
            }
            delete[] buffer;
        }
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    std::size_t jsonStart = response.find("\r\n\r\n");
    if (jsonStart != std::string::npos)
    {
        result = response.substr(jsonStart + 4);
    }

    result =  response;

	std::string hash = json::get_value(result, "hash");

	return hash;
}

std::string json::get_value(const std::string& json, const std::string& key) {
    std::size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";

    std::size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) return "";

    std::size_t valueStart = json.find_first_of("\"{[0123456789tf", colonPos);
    if (valueStart == std::string::npos) return "";

    char endChar = json[valueStart] == '"' ? '"' : json[valueStart] == '{' || json[valueStart] == '[' ? ']' : ',';

    std::size_t valueEnd = json.find(endChar, valueStart + 1);
    if (valueEnd == std::string::npos) return "";

    std::string value = json.substr(valueStart, valueEnd - valueStart + (endChar == '"' ? 1 : 0));

    if (value[0] == '"') value = value.substr(1, value.length() - 2);
    return value;
}