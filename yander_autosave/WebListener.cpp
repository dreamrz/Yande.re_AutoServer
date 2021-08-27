#include "stdafx.h"
#include "WebListener.h"
using namespace httplib;

WEB::WEB(int port)
{
	WebPort = port;
}

void WEB::HttpInterface(void)
{
    //用户登录接口
    svr.Post("/login", [&](const Request& req, Response& res) {
        Sleep(2000);//这是一个很好的防暴力破解的最简单方法,复杂的也不想去弄了.把并发降下来就是了
        if (req.body.at(0) == '{')
        {
            Json::CharReaderBuilder b;
            Json::CharReader* reader(b.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            bool ok = reader->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &root, &errs);
            if (ok && errs.size() == 0)
            {
                if (!root["user"].isNull() && !root["password"].isNull())
                {
                    if (root["user"].isString() && root["password"].isString())
                    {
                        SQL::LOGIN d = SQL::login(UTF8_To_string(root["user"].asString()).c_str(), UTF8_To_string(root["password"].asString()).c_str());
                        if (d.tokey.size())
                        {
                            std::string sendtext = "{\"error\":0,\"tokey\":\"";
                            sendtext += d.tokey;
                            sendtext += "\",\"memorandum\":\"";
                            sendtext += d.memorandum;
                            sendtext += "\"}";
                            res.set_content(string_To_UTF8(sendtext).c_str(), "text/plain");
                        }
                        else
                            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                    }
                    else
                        res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
                }
                else
                    res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
            }
            else
                res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
        }
        else
            res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
        });

    //更新备忘录
    svr.Post("/update", [&](const Request& req, Response& res) {
        if (req.body.at(0) == '{')
        {
            Json::CharReaderBuilder b;
            Json::CharReader* reader(b.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            bool ok = reader->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &root, &errs);
            if (ok && errs.size() == 0)
            {
                if (!root["user"].isNull() && !root["tokey"].isNull() && !root["memorandum"].isNull())
                {
                    if (root["user"].isString() && root["tokey"].isString() && root["memorandum"].isString())
                    {
                        if (SQL::updatememorandum(UTF8_To_string(root["user"].asString()).c_str(), UTF8_To_string(root["tokey"].asString()).c_str(), UTF8_To_string(root["memorandum"].asString()).c_str()))
                            res.set_content(string_To_UTF8("{\"error\":0}"), "text/plain");
                        else
                            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                    }
                    else
                        res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                }
                else
                    res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
            }
            else
                res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        }
        else
            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        });

    //提交一个请求码进入队列
    svr.Post("/add", [&](const Request& req, Response& res) {
        if (req.body.at(0) == '{')
        {
            Json::CharReaderBuilder b;
            Json::CharReader* reader(b.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            bool ok = reader->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &root, &errs);
            if (ok && errs.size() == 0)
            {
                if (!root["user"].isNull() && !root["tokey"].isNull() && !root["number"].isNull())
                {
                    if (root["user"].isString() && root["tokey"].isString() && root["number"].isInt())
                    {
                        bool s = SQL::addnewnumber(UTF8_To_string(root["user"].asString()).c_str(), UTF8_To_string(root["tokey"].asString()).c_str(), root["number"].asInt());
                        int ss;
                        s ? ss = 0 : ss = 1;
                        res.set_content(string_To_UTF8(std::string("{\"error\":") + std::to_string(ss) + "}"), "text/plain");
                    }
                    else
                        res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
                }
                else
                    res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
            }
            else
                res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
        }
        else
            res.set_content(string_To_UTF8("{\"error\":2}"), "text/plain");
        });
    
    //查询当前状态
    svr.Post("/system", [&](const Request& req, Response& res) {
        if (req.body.at(0) == '{')
        {
            Json::CharReaderBuilder b;
            Json::CharReader* reader(b.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            bool ok = reader->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &root, &errs);
            if (ok && errs.size() == 0)
            {
                if (!root["user"].isNull() && !root["tokey"].isNull())
                {
                    if (root["user"].isString() && root["tokey"].isString())
                    {
                        SQL::STATISTICAL s = SQL::getimage(UTF8_To_string(root["user"].asString()).c_str(), UTF8_To_string(root["tokey"].asString()).c_str());
                        std::string sendtext = "{\"error\":";
                        sendtext += std::to_string(s.error);
                        sendtext += ",\"all\":";
                        sendtext += std::to_string(s.all);
                        sendtext += ",\"analyse\":";
                        sendtext += std::to_string(s.analyse);
                        sendtext += ",\"download\":";
                        sendtext += std::to_string(s.download);
                        sendtext += "}";
                        res.set_content(string_To_UTF8(sendtext), "text/plain");
                    }
                    else
                        res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                }
                else
                    res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
            }
            else
                res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        }
        else
            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        });

    //修改用户名和密码
    svr.Post("/admin", [&](const Request& req, Response& res) {
        Sleep(2000);//简单防御一下
        if (req.body.at(0) == '{')
        {
            Json::CharReaderBuilder b;
            Json::CharReader* reader(b.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            bool ok = reader->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &root, &errs);
            if (ok && errs.size() == 0)
            {
                if (!root["oldUser"].isNull() && !root["oldPassword"].isNull() && !root["newUser"].isNull() && !root["newPassword"].isNull())
                {
                    if (root["oldUser"].isString() && root["oldPassword"].isString() && root["newUser"].isString() && root["newPassword"].isString())
                    {
                        bool s = SQL::changeuser(UTF8_To_string(root["oldUser"].asString()).c_str(), UTF8_To_string(root["oldPassword"].asString()).c_str(), UTF8_To_string(root["newUser"].asString()).c_str(), UTF8_To_string(root["newPassword"].asString()).c_str());
                        if (s)
                            res.set_content(string_To_UTF8("{\"error\":0}"), "text/plain");
                        else
                            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                    }
                    else
                        res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
                }
                else
                    res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
            }
            else
                res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        }
        else
            res.set_content(string_To_UTF8("{\"error\":1}"), "text/plain");
        });
}
