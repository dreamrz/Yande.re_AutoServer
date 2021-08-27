/*
* ע��������������������Ǳ��̳�,����Ҫ����һ������ʵ��
*/
#pragma once
#include "ReadCFG.hpp"

class SQL :private READCFG
{
public:
	struct LOGIN
	{
		std::string tokey;//����
		std::string memorandum;//����¼
	};
	struct STATISTICAL
	{
		int error;
		int all;
		int analyse;
		int download;
	};

public:
	SQL();
	~SQL();

	//�������ݿ�ӿڸ�web������ʹ�õ�
	LOGIN login(const char* user, const char* pwd);//��ѯ��¼,��������,��¼ʧ�ܷ��ؿ�����
	bool updatememorandum(const char* user, const char* tokey, const char* memorandum);//������־ û���û���ʱ����false
	bool addnewnumber(const char* user, const char* tokey, int number);//����һ��ͼƬ���, �Ѿ����ڷ���false
	STATISTICAL getimage(const char* user, const char* tokey);// ��ѯ��ǰͼƬ��ͳ����Ϣ
	bool changeuser(const char* ouser, const char* opwd, const char* nuser, const char* npwd);//�޸��û���������
	
	//�������ݿ�ӿڸ��ڲ������߼�ʹ�õ�
	int getanalysenumber(void);//��ȡһ����ûץȡurl�ı��,���账���򷵻�0
	std::string getdownloadnumber(int& number, std::string &filename);//��ȡһ����û���ص�url��ַ,���账��number�򷵻�0
	void setretries(int number);//ͨ���������һ��ʧ�ܴ���,������ץȡ��������ʧ��
	void setimagedata(int number, const char* filename, const char* imageurl);//��ȡ���ݺ�ͨ���������ͼƬ����
	void setimagedone(int number);//����ͼƬ��ͨ���������ͼƬ�������

	//�����ǰ�װ����ʹ�õ�,�����ݿ��ﴴ����,���ݿ����ֱ�����yande
	void createtable(void);

private:
	SQLHENV henv = SQL_NULL_HENV;
	SQLHDBC hdbc = SQL_NULL_HDBC;
	SQLHSTMT hstmt = SQL_NULL_HSTMT; //ÿ����ֻ��һ��������

	RETCODE retcode;
	std::wstring sqlcmd;
	LOGIN LoginOutMsg;


	std::wstring s2ws(const std::string& s);
	std::string TCHAR2STRING(TCHAR* STR)
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
		std::string str(chRtn);
		return str;
	}
};