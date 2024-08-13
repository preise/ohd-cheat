#pragma once

#include "Windows.h"
#include <iostream>
#include <tlhelp32.h>

#include <wininet.h>
#include <winhttp.h>
#include <shlwapi.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winhttp.lib")