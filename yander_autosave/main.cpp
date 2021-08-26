#include "stdafx.h"
#include "Server.h"
#include "SQL.h"

std::string TCHAR2STRING(TCHAR* STR) //宽字符转多字节 
{
    int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
    std::string str(chRtn);
    return str;
}

void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);

TCHAR szServiceName[] = _T("yande");
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;

SERVER server;

int main(int argc, char* argv[])
{
    if (argc >= 2)
    {
        if (std::string(argv[1]).find("-instllsql") != std::string::npos)
        {
            std::cout << "\n\n开始安装数据库源和表.\n";
            SQL sql;
            sql.createtable();
            std::cout << "完成.\n";
        }
        else if (std::string(argv[1]).find("-direct") != std::string::npos)
        {
            server.Run();
        }
        else if (std::string(argv[1]).find("-instll") != std::string::npos)
        {
            TCHAR szFilePath[MAX_PATH + 1] = { 0 };
            GetModuleFileName(NULL, szFilePath, MAX_PATH);
            (_tcsrchr(szFilePath, _T('\\')))[1] = 0;
            std::string APP_WORKPATH = TCHAR2STRING(szFilePath);
            std::string cmd = "sc create yande_re binpath= \"";
            cmd += APP_WORKPATH;
            cmd += "yander_autosave.exe\" displayname= \"yande_re下载服务端\" depend= Tcpip start= auto";
            system(cmd.c_str());
        }
        else if (std::string(argv[1]).find("-uninstll") != std::string::npos)
        {
            system("sc delete yande_re");
            std::cout << "\n注意需要重启电脑.\n";
        }
        else
            std::cout << "参数介绍:\n-instllsql  第一次安装数据库表.\n-direct  不注册服务运行.\n-instll  注册到服务组件里. (请至服务组件里手动设置该服务的\"登录\"里设置当前用户名和密码)\n-uninstll 卸载服务组件.\n";
        system("PAUSE");
    }
    else
    {
        SERVICE_TABLE_ENTRY st[] = { { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },{ NULL, NULL } };
        StartServiceCtrlDispatcher(st);
    }
    return 0;
}


void WINAPI ServiceMain()
{
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    SetServiceStatus(hServiceStatus, &status);
    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;


    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &status);

    server.Run();

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
}
 
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        server.stop();
        status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatus, &status);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        server.stop();
        status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatus, &status);
        break;
    default:
        break;
    }
}