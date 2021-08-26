/*
B/S 的WEB通信
除了访问网页都是get请求,发送的命令都是post请求
文本内容都是json数据,详细定义见cpp文件
*/
#pragma once
#include "SQL.h"

class WEB :private SQL
{
public:
	WEB(int port);

	bool run(void)
	{
		//获取exe当前环境目录,如果是服务组件形式运行需要拿到自身的目录
		TCHAR szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
		std::string APP_WORKPATH = TCHAR2STRING(szFilePath);

		svr.set_mount_point("/", std::string(APP_WORKPATH + "wwwroot").c_str()); //定位html资源到这个路径
		HttpInterface();
		svr.set_read_timeout(10);
		svr.set_write_timeout(10);
		svr.set_payload_max_length(2048); //限制接收2kb字节内容
		svr.set_keep_alive_max_count(1);//并发只能有一个!!! 后台用户通常就一个私有使用
		//注意这里设计继承到sql,但sql类的实例只有一个,这意味所有并发用户都用这一个实例访问数据库!!!!!
		//不愿意在每个并发里实例一个sql,会有大量的io申请.另外也不想做的太复杂了
		return svr.listen("0.0.0.0", WebPort);
	}

	void stop()
	{
		svr.stop();
	}

private:
	int WebPort;
	httplib::Server svr;

	void HttpInterface(void);

	std::string TCHAR2STRING(TCHAR* STR) //宽字符转多字节 
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
		std::string str(chRtn);
		return str;
	}

	std::string UTF8_To_string(const std::string& str) //utf8转ascii
	{
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1]; 
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr = pBuf;
		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return retStr;
	}

	std::string string_To_UTF8(const std::string& str) //ascii转utf8
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}

};