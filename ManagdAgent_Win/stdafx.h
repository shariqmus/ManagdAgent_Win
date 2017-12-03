// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

//Additional Windows Includes
#include <ATLComTime.h>
#include <wuapi.h>
#include <wuerror.h>
#include <windows.h>
#include <wincrypt.h>
#include <cryptuiapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <shlwapi.h>
#include <Pdh.h>

// C/C++ Includes
#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdio.h> 
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <vector>
using namespace std;

// Custom Includes

// jsonxx -- https://github.com/hjiang/jsonxx
#include "jsonxx.h"
using namespace jsonxx;

#include "WinReg.hpp" // https://github.com/GiovanniDicanio/WinReg
using namespace winreg;

// Logging
#include "log.h"

// Config file reader
#include "INIReader.h"

// cURL
#include "curl/curl.h"

//Timer
#include "cxxtimer.h"

//libcurl.lib;crypt32.lib
#pragma comment(lib, "..\\curl-7.46.0-win64\\lib\\libcurl.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "cryptui.lib")
#pragma comment(lib, "pdh.lib")