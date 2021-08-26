/*
READCFG 接口
bool Done(void);//返回读取配置是否完成
configuration GetCFG(void);//返回各种配置信息
*/

#pragma once

class READCFG
{
private:
	struct configuration
	{
		char ImageRootPath[512]; //存放图片的根目录
		int WebPort; //网页端口
		char ODBCname[128]; //odbc源
		char ODBCuser[128]; //odbc用户
		char ODBCpassword[128]; //odbc密码
		int WorkTimeSec; //检测新数据超时
		int DownloadTimeoutSec; //下载超时
		int LoginTimeoutSec; //登录动作超时
		char proxy[512];//代理服务器地址
	}cfg;

public:
	READCFG()
	{
		//获取exe当前环境目录,如果是服务组件形式运行需要拿到自身的目录
		TCHAR szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
		std::string APP_WORKPATH = TCHAR2STRING(szFilePath);

		ReadIsDone = false;
		memset(cfg.ImageRootPath, 0, sizeof(cfg.ImageRootPath));

		std::ifstream in(APP_WORKPATH + "cfg.json");
		if (!in) { printf("无法读取cfg.json文件\n"); return; }
		std::istreambuf_iterator<char> beg(in), end;
		std::string str(beg, end);
		in.clear();
		in.close();

		Json::CharReaderBuilder b;
		Json::CharReader* reader(b.newCharReader());
		Json::Value root;
		JSONCPP_STRING errs;
		bool ok = reader->parse(str.c_str(), str.c_str() + str.size(), &root, &errs);
		if (ok && errs.size() == 0)
		{
			std::string temp;
			if (root["ImageRootPath"].isNull()) { printf("无法读取cfg.json中ImageRootPath项\n"); return; }
			if (!root["ImageRootPath"].isString()) { printf("ImageRootPath项数据类型错误,不是字符串\n"); return; }
			temp = root["ImageRootPath"].asString();
			if (temp.size() < 512)
				memcpy(cfg.ImageRootPath, temp.c_str(), temp.size());
			else
			{
				printf("ImageRootPath超过512个字符\n"); return;
			}

			if (root["WebPort"].isNull()) { printf("无法读取cfg.json中WebPort项\n"); return; }
			if (!root["WebPort"].isInt()) { printf("WebPort项数据类型错误,不是整形\n"); return; }
			cfg.WebPort = root["WebPort"].asInt();

			if (root["ODBCname"].isNull()) { printf("无法读取cfg.json中ODBCname项\n"); return; }
			if (!root["ODBCname"].isString()) { printf("ODBCname项数据类型错误,不是字符串\n"); return; }
			temp = root["ODBCname"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCname, temp.c_str(), temp.size());
			else
			{
				printf("ODBCname超过128个字符\n"); return;
			}

			if (root["ODBCuser"].isNull()) { printf("无法读取cfg.json中ODBCuser项\n"); return; }
			if (!root["ODBCuser"].isString()) { printf("ODBCuser项数据类型错误,不是字符串\n"); return; }
			temp = root["ODBCuser"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCuser, temp.c_str(), temp.size());
			else
			{
				printf("ODBCuser超过128个字符\n"); return;
			}

			if (root["ODBCpassword"].isNull()) { printf("无法读取cfg.json中ODBCpassword项\n"); return; }
			if (!root["ODBCpassword"].isString()) { printf("ODBCpassword项数据类型错误,不是字符串\n"); return; }
			temp = root["ODBCpassword"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCpassword, temp.c_str(), temp.size());
			else
			{
				printf("ODBCpassword超过128个字符\n"); return;
			}

			if (root["WorkTimeSec"].isNull()) { printf("无法读取cfg.json中WorkTimeSec项\n"); return; }
			if (!root["WorkTimeSec"].isInt()) { printf("WorkTimeSec项数据类型错误,不是整形\n"); return; }
			cfg.WorkTimeSec = root["WorkTimeSec"].asInt();

			if (root["DownloadTimeoutSec"].isNull()) { printf("无法读取cfg.json中DownloadTimeoutSec项\n"); return; }
			if (!root["DownloadTimeoutSec"].isInt()) { printf("DownloadTimeoutSec项数据类型错误,不是整形\n"); return; }
			cfg.DownloadTimeoutSec = root["DownloadTimeoutSec"].asInt();

			if (root["LoginTimeoutSec"].isNull()) { printf("无法读取cfg.json中LoginTimeoutSec项\n"); return; }
			if (!root["LoginTimeoutSec"].isInt()) { printf("LoginTimeoutSec项数据类型错误,不是整形\n"); return; }
			cfg.LoginTimeoutSec = root["LoginTimeoutSec"].asInt();

			memset(cfg.proxy, 0, sizeof(cfg.proxy));
			if (root["Proxy"].isNull()) { printf("无法读取cfg.json中Proxy项\n"); return; }
			if (!root["Proxy"].isString()) { printf("Proxy项数据类型错误,不是字符串\n"); return; }
			temp = root["Proxy"].asString();
			if (temp.size() < 512)
				memcpy(cfg.proxy, temp.c_str(), temp.size());
			else
			{
				printf("Proxy超过512个字符\n"); return;
			}
		}
		ReadIsDone = true;
	}

	//读取配置文件是否成功
	bool Done(void)
	{
		return ReadIsDone;
	}

	//获取配置文件详细数据
	configuration GetCFG(void)
	{
		return cfg;
	}

private:
	bool ReadIsDone;

	
	std::string TCHAR2STRING(TCHAR* STR) //宽字符转多字节 
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
		std::string str(chRtn);
		return str;
	}
};