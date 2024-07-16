#pragma once

#include "Windows.h"
#include <iostream>
#include <tlhelp32.h>

#include <wininet.h>
#include <shlwapi.h>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shlwapi.lib")