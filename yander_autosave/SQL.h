/*
* 注意这个类无论是声明还是被继承,都不要用在一个以上实例
*/
#pragma once
#include "ReadCFG.hpp"

class SQL :private READCFG
{
public:
	struct LOGIN
	{
		std::string tokey;//令牌
		std::string memorandum;//备忘录
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

	//下面数据库接口给web端请求使用的
	LOGIN login(const char* user, const char* pwd);//查询登录,返回令牌,登录失败返回空令牌
	bool updatememorandum(const char* user, const char* tokey, const char* memorandum);//更新日志 没有用户或超时返回false
	bool addnewnumber(const char* user, const char* tokey, int number);//增加一个图片编号, 已经存在返回false
	STATISTICAL getimage(const char* user, const char* tokey);// 查询当前图片的统计信息
	bool changeuser(const char* ouser, const char* opwd, const char* nuser, const char* npwd);//修改用户名和密码
	
	//下面数据库接口给内部处理逻辑使用的
	int getanalysenumber(void);//获取一个还没抓取url的编号,无需处理则返回0
	std::string getdownloadnumber(int& number, std::string &filename);//获取一个还没下载的url地址,无需处理number则返回0
	void setretries(int number);//通过编号增加一个失败次数,无论是抓取或是下载失败
	void setimagedata(int number, const char* filename, const char* imageurl);//获取数据后通过编号设置图片数据
	void setimagedone(int number);//下载图片后通过编号设置图片下载完成

	//下面是安装程序使用的,在数据库里创建表,数据库名字必须是yande
	void createtable(void);

private:
	SQLHENV henv = SQL_NULL_HENV;
	SQLHDBC hdbc = SQL_NULL_HDBC;
	SQLHSTMT hstmt = SQL_NULL_HSTMT; //每个类只做一个事务句柄

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