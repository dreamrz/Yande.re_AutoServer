#pragma once
#include "ReadCFG.hpp"
#include "WebListener.h"

class SERVER :private READCFG
{
public:
	SERVER() :web(READCFG::GetCFG().WebPort)
	{
		if (!READCFG::Done())
		{
			printf("读取配置失败.\n");
			return;
		}
	}

	~SERVER()
	{
		web.stop();
		APP = false;
	}

	bool Run(void)
	{
		APP = true;
		_beginthreadex(nullptr, 0, this->AnalyseThread, this, 0, nullptr);
		//_beginthreadex(nullptr, 0, this->DownloadThread, this, 0, nullptr);//libcurl居然不是线程安全的,这里合并到上面去了

		if (!web.run()) //最后一个启动
		{
			printf("http启动失败,也许是端口被占用了.\n");
			return false;
		}
		return true;
	}

	void stop(void)
	{
		web.stop();
	}

private:
	WEB web;

	bool APP;
	static unsigned int __stdcall AnalyseThread(void* pM);//抓取链接线程
	static unsigned int __stdcall DownloadThread(void* pM);//下载线程

	static size_t analysecallback(char* ptr, size_t size, size_t nmemb, std::string* html);

	std::string UTF8_To_string(const std::string& str)
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

	struct image_d //有stl 这个不是拿来拷贝的
	{
		bool succeed;//是否完成
		std::string url; //拿到的地址
		std::string name; //拿到的文件名
	};

	image_d GetUrl(std::string html);
};