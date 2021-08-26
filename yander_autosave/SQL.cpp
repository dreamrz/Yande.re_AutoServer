#include "stdafx.h"
#include "SQL.h"
#include "MD5.h"
#include <odbcinst.h>
#pragma comment(lib, "legacy_stdio_definitions.lib")

SQL::SQL()
{
    //每次都会尝试创建odbc
    std::wstring SQLAddress = L"127.0.0.1";
    std::wstring SQLodbc = s2ws(READCFG::GetCFG().ODBCname);
    std::wstring bcmd = L"Server=" + SQLAddress + L"?DSN=" + SQLodbc + L"??";
    wchar_t sqlcmd[128];
    wmemcpy(sqlcmd, bcmd.c_str(), bcmd.size());
    for (int i = 0; i < 128; i++)
    {
        if (sqlcmd[i] == '?')sqlcmd[i] = '\0';
    }
    SQLConfigDataSource(NULL, ODBC_ADD_DSN, L"SQL Server", sqlcmd); //已存在失败会false 不用理会




	if (!READCFG::Done())return;
    std::wstring szDSN = s2ws(READCFG::GetCFG().ODBCname);
    std::wstring userID = s2ws(READCFG::GetCFG().ODBCuser);
    std::wstring passWORD = s2ws(READCFG::GetCFG().ODBCpassword);

    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (SQL_SUCCESS != retcode)
    {
        printf("SQLAllocHandle失败.\n");
        return;
    }
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    if (SQL_SUCCESS != retcode)
    {
        printf("SQLSetEnvAttr失败.\n");
        return;
    }
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (SQL_SUCCESS != retcode)
    {
        printf("SQLAllocHandle失败.\n");
        return;
    }
    retcode = SQLConnect(hdbc, (SQLWCHAR*)szDSN.c_str(), SQL_NTS, (SQLWCHAR*)userID.c_str(), SQL_NTS, (SQLWCHAR*)passWORD.c_str(), SQL_NTS);
    if (retcode != 1)
    {
        printf("SQLConnect失败.\n");
        return;
    }
}

SQL::~SQL()
{
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

}

SQL::LOGIN SQL::login(const char* user, const char* pwd)
{
    LoginOutMsg.tokey = "";
    LoginOutMsg.memorandum = "";

    sqlcmd = L"SELECT [memorandum] FROM [yandere].[dbo].[LoginUser] WHERE [loginuser] = '";
    sqlcmd += s2ws(user);
    sqlcmd += L"' AND [loginpassword] = '";
    sqlcmd += s2ws(pwd);
    sqlcmd += L"';";

    char memorandum[4096];
    memset(memorandum, 0, sizeof(memorandum));
    SQLLEN memorandumSize;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            //只做一次判断,即便有其它相同数据
            SQLRETURN sqltest = SQLFetch(hstmt);
            if (SQL_NO_DATA != sqltest && SQL_ERROR != sqltest)
            {
                SQLGetData(hstmt, 1, SQL_C_CHAR, memorandum, 4096, &memorandumSize);
                memorandum[4095] = 0;//断开,只留这些长度
                MD5 md5(std::string(std::string(user) + std::to_string((unsigned)time(nullptr)))); //用户名和当前时间戳简单合成令牌
                LoginOutMsg.tokey = md5.toString();
                LoginOutMsg.memorandum = memorandum;

                SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
                sqlcmd = L"UPDATE [yandere].[dbo].[LoginUser] SET [tokey]='";
                sqlcmd += s2ws(LoginOutMsg.tokey);
                sqlcmd += L"',[time] = GETDATE() WHERE [loginuser]='";
                sqlcmd += s2ws(user);
                sqlcmd += L"';";
                SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
            }
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }

    return LoginOutMsg;
}

bool SQL::updatememorandum(const char* user, const char* tokey, const char* memorandum)
{
    bool s = true;
    sqlcmd = L"UPDATE[yandere].[dbo].[LoginUser] SET[memorandum] = '";
    sqlcmd += s2ws(memorandum);
    sqlcmd += L"', [time] = GETDATE() WHERE[loginuser] = '";
    sqlcmd += s2ws(user);
    sqlcmd += L"' AND[tokey] = '";
    sqlcmd += s2ws(tokey);
    sqlcmd += L"' AND(DATEDIFF(second, '1970-01-01 08:00:00', (GETDATE())) - DATEDIFF(second, '1970-01-01 08:00:00', ([time])) < ";
    sqlcmd += s2ws(std::to_string(READCFG::GetCFG().LoginTimeoutSec));
    sqlcmd += L");";

    SQLLEN cbstate;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            SQLRowCount(hstmt, &cbstate);
            cbstate >= 1 ? s = true : s = false; //影响一条以上说明成功

        }
        else
            s = false;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else
        s = false;
    
    return s;
}

bool SQL::addnewnumber(const char* user, const char* tokey, int number)
{
    bool s = true;
    //这里不写储存过程了,直接用语句操作两个表
    //先查询用户令牌,然后查询编号是否存在,如果不存在则增加图片表新编号并重设用户表时间戳
    sqlcmd = L"DECLARE @username nvarchar(50) SET @username = '";
    sqlcmd += s2ws(user);
    sqlcmd += L"'\r\nDECLARE @tokey nvarchar(32) SET @tokey = '";
    sqlcmd += s2ws(tokey);
    sqlcmd += L"'\r\nDECLARE @timeout int SET @timeout = ";
    sqlcmd += s2ws(std::to_string(READCFG::GetCFG().LoginTimeoutSec));
    sqlcmd += L"\r\nDECLARE @image int SET @image = ";
    sqlcmd += s2ws(std::to_string(number));
    sqlcmd += L"\r\nDECLARE @count int\r\n";
    sqlcmd += L"IF exists(SELECT * FROM [yandere].[dbo].[LoginUser] WHERE [loginuser] = @username AND [tokey] = @tokey AND (DATEDIFF(second,'1970-01-01 08:00:00', (GETDATE())) - DATEDIFF(second,'1970-01-01 08:00:00', ([time])) < @timeout))\r\n";
    sqlcmd += L"IF exists(SELECT * FROM [yandere].[dbo].[ImageData] WHERE [PostNumber] = @image)\r\n";
    sqlcmd += L"SET @count = 0\r\n";
    sqlcmd += L"ELSE\r\n";
    sqlcmd += L"BEGIN\r\n";
    sqlcmd += L"INSERT INTO [yandere].[dbo].[ImageData] ([FileName], [PostNumber], [ImageUrl], [Analyse], [Download], [Retries]) VALUES ('', @image, '', 0, 0, 0)\r\n";
    sqlcmd += L"UPDATE [yandere].[dbo].[LoginUser] SET [time] = GETDATE() WHERE [loginuser] = @username\r\n";
    sqlcmd += L"END\r\n";

    SQLLEN cbstate;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            SQLRowCount(hstmt, &cbstate);
            cbstate >= 1 ? s = true : s = false; //影响一条以上说明成功

        }
        else
            s = false;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else
        s = false;

    return s;
}

SQL::STATISTICAL SQL::getimage(const char* user, const char* tokey)
{
    //先用用户表中查询令牌和是否超时
    //然后去图片表做统计
    STATISTICAL statistical;
    memset(&statistical, 0, sizeof(STATISTICAL));
    sqlcmd = L"DECLARE @username nvarchar(50) SET @username = '";
    sqlcmd += s2ws(user);
    sqlcmd += L"'\r\nDECLARE @tokey nvarchar(32) SET @tokey = '";
    sqlcmd += s2ws(tokey);
    sqlcmd += L"'\r\nDECLARE @timeout int SET @timeout = ";
    sqlcmd += s2ws(std::to_string(READCFG::GetCFG().LoginTimeoutSec));
    sqlcmd += L"\r\nIF exists(SELECT * FROM [yandere].[dbo].[LoginUser] WHERE [loginuser] = @username AND [tokey] = @tokey AND (DATEDIFF(second,'1970-01-01 08:00:00', (GETDATE())) - DATEDIFF(second,'1970-01-01 08:00:00', ([time])) < @timeout))\r\n"
        L"BEGIN\r\n"
        L"DECLARE @ALL int\r\n"
        L"DECLARE @ANALYSE int\r\n"
        L"DECLARE @DOWNLOAD int\r\n"
        L"SELECT @ALL = COUNT(*) FROM[yandere].[dbo].[ImageData]\r\n"
        L"SELECT @ANALYSE = COUNT(*) FROM[yandere].[dbo].[ImageData] WHERE[Analyse] != 0\r\n"
        L"SELECT @DOWNLOAD = COUNT(*) FROM[yandere].[dbo].[ImageData] WHERE[Download] != 0\r\n"
        L"SELECT @ALL, @ANALYSE, @DOWNLOAD\r\n"
        L"END";

    SQLLEN all, anaylyse, download;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            SQLRETURN sqltest = SQLFetch(hstmt);
            if (SQL_NO_DATA != sqltest && SQL_ERROR != sqltest)
            {
                SQLGetData(hstmt, 1, SQL_C_LONG, &statistical.all, 0, &all);
                SQLGetData(hstmt, 2, SQL_C_LONG, &statistical.analyse, 0, &anaylyse);
                SQLGetData(hstmt, 3, SQL_C_LONG, &statistical.download, 0, &download);
                statistical.error = 0;
            }
            else
                statistical.error = 1;
        }
        else
            statistical.error = 1;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else
        statistical.error = 1;

    return statistical;
}

int SQL::getanalysenumber(void)
{
    int p_n = 0;
    SQLLEN cbN;
    sqlcmd = L"SELECT TOP(1) [PostNumber] FROM [yandere].[dbo].[ImageData] WHERE [Analyse] = 0 ORDER BY convert(int,[Retries]);";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            SQLRETURN sqltest = SQLFetch(hstmt);
            if (SQL_NO_DATA != sqltest && SQL_ERROR != sqltest)
            {
                SQLGetData(hstmt, 1, SQL_C_LONG, &p_n, 0, &cbN);
            }
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    return p_n;
}

std::string SQL::getdownloadnumber(int& number, std::string& filename)
{
    number = 0;
    char p_url[500];
    char p_name[200];
    SQLLEN cbN;
    sqlcmd = L"SELECT TOP(1) [ImageUrl], [PostNumber], [FileName] FROM [yandere].[dbo].[ImageData] WHERE [Download] = 0 AND [Analyse] = 1 ORDER BY convert(int,[Retries]);";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        if (SQL_SUCCESS == retcode)
        {
            SQLRETURN sqltest = SQLFetch(hstmt);
            if (SQL_NO_DATA != sqltest && SQL_ERROR != sqltest)
            {
                memset(p_url, 0, sizeof(p_url));
                SQLGetData(hstmt, 1, SQL_C_CHAR, &p_url, 500, &cbN);
                SQLGetData(hstmt, 2, SQL_C_LONG, &number, 0, &cbN);
                SQLGetData(hstmt, 3, SQL_C_CHAR, &p_name, 200, &cbN);
                filename = p_name;
            }
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    p_url[499] = 0;
    return std::string(p_url);
}

void SQL::setretries(int number)
{
    sqlcmd = L"UPDATE [yandere].[dbo].[ImageData] SET [Retries] = [Retries] + 1 WHERE [PostNumber] = ";
    sqlcmd += s2ws(std::to_string(number));
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
}

void SQL::setimagedata(int number, const char* filename, const char* imageurl)
{
    sqlcmd = L"DECLARE @number int SET @number = ";
    sqlcmd += s2ws(std::to_string(number));
    sqlcmd += L"\r\nDECLARE @filename nvarchar(200) SET @filename = '";
    sqlcmd += s2ws(filename);
    sqlcmd += L"'\r\nDECLARE @url nvarchar(500) SET @url = '";
    sqlcmd += s2ws(imageurl);
    sqlcmd += L"'\r\nUPDATE [yandere].[dbo].[ImageData] SET [FileName]=@filename, [ImageUrl] = @url, [Analyse] = 1 WHERE [PostNumber] = @number";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
}

void SQL::setimagedone(int number)
{
    sqlcmd = L"UPDATE [yandere].[dbo].[ImageData] SET [Download] = 1 WHERE [PostNumber] = ";
    sqlcmd += s2ws(std::to_string(number));
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
}

void SQL::createtable(void)
{
    //----------第一步创建数据库----------
    //注意数据库和服务端要安装在同一台机器上
    //这里使用程序所在路径,而数据库则创建在数据库服务器上
    //这意味如果数据库的机器上不存在程序机器上的路径则会失败
    TCHAR szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileName(NULL, szFilePath, MAX_PATH);
    (_tcsrchr(szFilePath, _T('\\')))[1] = 0;
    std::string APP_WORKPATH = TCHAR2STRING(szFilePath);

    sqlcmd = L"CREATE DATABASE yandere\r\n"
        L"on(name = yandere,filename = '";
    sqlcmd += s2ws(APP_WORKPATH) + L"yandere_data.mdf',size = 10MB,maxsize = 100MB,filegrowth = 10%)\r\n";
    sqlcmd += L"log on(name = yandere_log,filename = '";
    sqlcmd += s2ws(APP_WORKPATH) + L"yandere_log.ldf',size=2MB, maxsize = 5MB,filegrowth = 10%);";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    //----------第二步创建表----------
    sqlcmd = L"USE yandere\r\n"
        L"CREATE TABLE dbo.LoginUser\r\n"
        L"(\r\n"
        L"    [loginuser] nvarchar(50) NOT NULL,\r\n"
        L"    [loginpassword] nvarchar(32) NOT NULL,\r\n"
        L"    [memorandum] nvarchar(MAX) NULL DEFAULT '',\r\n"
        L"    [tokey] nvarchar(32) NOT NULL,\r\n"
        L"    [time] datetime NOT NULL\r\n"
        L");";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    sqlcmd = L"USE yandere\r\n"
        L"CREATE TABLE dbo.ImageData\r\n"
        L"(\r\n"
        L"    [id] int NOT NULL IDENTITY(1, 1),\r\n"
        L"    [FileName] nvarchar(200) NULL DEFAULT '',\r\n"
        L"    [PostNumber] int NOT NULL,\r\n"
        L"    [ImageUrl] nvarchar(500) NULL DEFAULT '',\r\n"
        L"    [Analyse] bit NOT NULL,\r\n"
        L"    [Download] bit NOT NULL,\r\n"
        L"    [Retries] int NOT NULL\r\n"
        L");";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    //----------第三步创建一个用户----------
    sqlcmd = L"INSERT INTO [yandere].[dbo].[LoginUser] ([loginuser], [loginpassword], [memorandum], [tokey], [time]) VALUES ('admin', '21232f297a57a5a743894a0e4a801fc3', '', '', GETDATE())";
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (SQL_SUCCESS == retcode)
    {
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sqlcmd.c_str(), SQL_NTSL);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
}

std::wstring SQL::s2ws(const std::string& s)
{
    std::string curLocale = setlocale(LC_ALL, "");
    const char* _Source = s.c_str();
    size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest, _Source, _Dsize);
    std::wstring result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;

}
