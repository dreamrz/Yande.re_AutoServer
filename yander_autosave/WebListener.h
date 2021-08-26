/*
B/S ��WEBͨ��
���˷�����ҳ����get����,���͵������post����
�ı����ݶ���json����,��ϸ�����cpp�ļ�
*/
#pragma once
#include "SQL.h"

class WEB :private SQL
{
public:
	WEB(int port);

	bool run(void)
	{
		//��ȡexe��ǰ����Ŀ¼,����Ƿ��������ʽ������Ҫ�õ������Ŀ¼
		TCHAR szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
		std::string APP_WORKPATH = TCHAR2STRING(szFilePath);

		svr.set_mount_point("/", std::string(APP_WORKPATH + "wwwroot").c_str()); //��λhtml��Դ�����·��
		HttpInterface();
		svr.set_read_timeout(10);
		svr.set_write_timeout(10);
		svr.set_payload_max_length(2048); //���ƽ���2kb�ֽ�����
		svr.set_keep_alive_max_count(1);//����ֻ����һ��!!! ��̨�û�ͨ����һ��˽��ʹ��
		//ע��������Ƽ̳е�sql,��sql���ʵ��ֻ��һ��,����ζ���в����û�������һ��ʵ���������ݿ�!!!!!
		//��Ը����ÿ��������ʵ��һ��sql,���д�����io����.����Ҳ��������̫������
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

	std::string TCHAR2STRING(TCHAR* STR) //���ַ�ת���ֽ� 
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
		std::string str(chRtn);
		return str;
	}

	std::string UTF8_To_string(const std::string& str) //utf8תascii
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

	std::string string_To_UTF8(const std::string& str) //asciiתutf8
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