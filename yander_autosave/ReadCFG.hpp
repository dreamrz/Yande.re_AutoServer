/*
READCFG �ӿ�
bool Done(void);//���ض�ȡ�����Ƿ����
configuration GetCFG(void);//���ظ���������Ϣ
*/

#pragma once

class READCFG
{
private:
	struct configuration
	{
		char ImageRootPath[512]; //���ͼƬ�ĸ�Ŀ¼
		int WebPort; //��ҳ�˿�
		char ODBCname[128]; //odbcԴ
		char ODBCuser[128]; //odbc�û�
		char ODBCpassword[128]; //odbc����
		int WorkTimeSec; //��������ݳ�ʱ
		int DownloadTimeoutSec; //���س�ʱ
		int LoginTimeoutSec; //��¼������ʱ
		char proxy[512];//�����������ַ
	}cfg;

public:
	READCFG()
	{
		//��ȡexe��ǰ����Ŀ¼,����Ƿ��������ʽ������Ҫ�õ������Ŀ¼
		TCHAR szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
		std::string APP_WORKPATH = TCHAR2STRING(szFilePath);

		ReadIsDone = false;
		memset(cfg.ImageRootPath, 0, sizeof(cfg.ImageRootPath));

		std::ifstream in(APP_WORKPATH + "cfg.json");
		if (!in) { printf("�޷���ȡcfg.json�ļ�\n"); return; }
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
			if (root["ImageRootPath"].isNull()) { printf("�޷���ȡcfg.json��ImageRootPath��\n"); return; }
			if (!root["ImageRootPath"].isString()) { printf("ImageRootPath���������ʹ���,�����ַ���\n"); return; }
			temp = root["ImageRootPath"].asString();
			if (temp.size() < 512)
				memcpy(cfg.ImageRootPath, temp.c_str(), temp.size());
			else
			{
				printf("ImageRootPath����512���ַ�\n"); return;
			}

			if (root["WebPort"].isNull()) { printf("�޷���ȡcfg.json��WebPort��\n"); return; }
			if (!root["WebPort"].isInt()) { printf("WebPort���������ʹ���,��������\n"); return; }
			cfg.WebPort = root["WebPort"].asInt();

			if (root["ODBCname"].isNull()) { printf("�޷���ȡcfg.json��ODBCname��\n"); return; }
			if (!root["ODBCname"].isString()) { printf("ODBCname���������ʹ���,�����ַ���\n"); return; }
			temp = root["ODBCname"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCname, temp.c_str(), temp.size());
			else
			{
				printf("ODBCname����128���ַ�\n"); return;
			}

			if (root["ODBCuser"].isNull()) { printf("�޷���ȡcfg.json��ODBCuser��\n"); return; }
			if (!root["ODBCuser"].isString()) { printf("ODBCuser���������ʹ���,�����ַ���\n"); return; }
			temp = root["ODBCuser"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCuser, temp.c_str(), temp.size());
			else
			{
				printf("ODBCuser����128���ַ�\n"); return;
			}

			if (root["ODBCpassword"].isNull()) { printf("�޷���ȡcfg.json��ODBCpassword��\n"); return; }
			if (!root["ODBCpassword"].isString()) { printf("ODBCpassword���������ʹ���,�����ַ���\n"); return; }
			temp = root["ODBCpassword"].asString();
			if (temp.size() < 128)
				memcpy(cfg.ODBCpassword, temp.c_str(), temp.size());
			else
			{
				printf("ODBCpassword����128���ַ�\n"); return;
			}

			if (root["WorkTimeSec"].isNull()) { printf("�޷���ȡcfg.json��WorkTimeSec��\n"); return; }
			if (!root["WorkTimeSec"].isInt()) { printf("WorkTimeSec���������ʹ���,��������\n"); return; }
			cfg.WorkTimeSec = root["WorkTimeSec"].asInt();

			if (root["DownloadTimeoutSec"].isNull()) { printf("�޷���ȡcfg.json��DownloadTimeoutSec��\n"); return; }
			if (!root["DownloadTimeoutSec"].isInt()) { printf("DownloadTimeoutSec���������ʹ���,��������\n"); return; }
			cfg.DownloadTimeoutSec = root["DownloadTimeoutSec"].asInt();

			if (root["LoginTimeoutSec"].isNull()) { printf("�޷���ȡcfg.json��LoginTimeoutSec��\n"); return; }
			if (!root["LoginTimeoutSec"].isInt()) { printf("LoginTimeoutSec���������ʹ���,��������\n"); return; }
			cfg.LoginTimeoutSec = root["LoginTimeoutSec"].asInt();

			memset(cfg.proxy, 0, sizeof(cfg.proxy));
			if (root["Proxy"].isNull()) { printf("�޷���ȡcfg.json��Proxy��\n"); return; }
			if (!root["Proxy"].isString()) { printf("Proxy���������ʹ���,�����ַ���\n"); return; }
			temp = root["Proxy"].asString();
			if (temp.size() < 512)
				memcpy(cfg.proxy, temp.c_str(), temp.size());
			else
			{
				printf("Proxy����512���ַ�\n"); return;
			}
		}
		ReadIsDone = true;
	}

	//��ȡ�����ļ��Ƿ�ɹ�
	bool Done(void)
	{
		return ReadIsDone;
	}

	//��ȡ�����ļ���ϸ����
	configuration GetCFG(void)
	{
		return cfg;
	}

private:
	bool ReadIsDone;

	
	std::string TCHAR2STRING(TCHAR* STR) //���ַ�ת���ֽ� 
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
		std::string str(chRtn);
		return str;
	}
};