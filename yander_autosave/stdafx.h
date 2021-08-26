#pragma once
#define CURL_STATICLIB

#include "httplib.h"
#include <Windows.h>
#include <process.h>
#include <sql.h> 
#include <sqlext.h> 
#include <sqltypes.h> 
#include <odbcss.h> 
#include <iostream>
#include <fstream>
#include <string>
#include <tchar.h>

#include "json/json.h"
#include "curl/curl.h"

#pragma comment(lib,"jsoncpp_static.lib")
#pragma comment(lib,"libcurl.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"Crypt32.lib")