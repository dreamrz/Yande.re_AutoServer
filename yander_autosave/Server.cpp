#include "stdafx.h"
#include "Server.h"
#include "SQL.h"

unsigned int __stdcall SERVER::AnalyseThread(void* pM)
{
	SQL sql;
	
	SERVER* T = (SERVER*)pM;
	while (T->APP)
	{
		while (true)
		{
			int number = sql.getanalysenumber();
			if (!number)break; //�жϵ���һ������ȥ
			//https://yande.re/post/show/
			std::string web = "https://yande.re/post/show/" + std::to_string(number);
			CURL* curl = curl_easy_init();
			if (curl)
			{
				char m_url[256];
				memset(m_url, 0, sizeof(m_url));
				memcpy(m_url, web.c_str(), web.size());
				std::string html;
				curl_easy_setopt(curl, CURLOPT_URL, m_url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, analysecallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
				curl_easy_setopt(curl, CURLOPT_TIMEOUT, T->READCFG::GetCFG().DownloadTimeoutSec);
				if (std::string(T->READCFG::GetCFG().proxy).size() > 0)
					curl_easy_setopt(curl, CURLOPT_PROXY, T->READCFG::GetCFG().proxy);
				CURLcode res = curl_easy_perform(curl);
				if (res == CURLE_OK)
				{
					curl_easy_cleanup(curl);
					//����html��������
					image_d d = T->GetUrl(html);
					if (d.succeed)
						sql.setimagedata(number, d.name.c_str(), d.url.c_str());
					else
					{
						sql.setretries(number);
						break;
					}
				}
				else
					sql.setretries(number);//ץȡʧ��,����һ��ʧ�ܴ���
				curl_global_cleanup();
			}
		}
		Sleep(T->READCFG::GetCFG().WorkTimeSec * 1000);//�ڳ��ź�֪ͨ����ʡ��Դ����
	}
	return 0;
}

unsigned int __stdcall SERVER::DownloadThread(void* pM)
{
	SQL sql;

	SERVER* T = (SERVER*)pM;
	while (T->APP)
	{
		while (true)
		{
			int number;
			std::string name;
			std::string url = sql.getdownloadnumber(number, name);
			if (!number)break; //�жϵ���һ������ȥ

			FILE* file = nullptr;
			fopen_s(&file, std::string(T->READCFG::GetCFG().ImageRootPath + name).c_str(), "wb+");//����ģʽ

			CURL* curl = curl_easy_init();
			if (curl && file != nullptr)
			{
				char m_url[256];
				memset(m_url, 0, sizeof(m_url));
				memcpy(m_url, url.c_str(), url.size());
				curl_easy_setopt(curl, CURLOPT_URL, m_url);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);

				curl_easy_setopt(curl, CURLOPT_TIMEOUT, T->READCFG::GetCFG().DownloadTimeoutSec);
				curl_easy_setopt(curl, CURLOPT_PROXY, T->READCFG::GetCFG().proxy);
				CURLcode res = curl_easy_perform(curl);
				if (res == CURLE_OK)
				{
					curl_easy_cleanup(curl);
					sql.setimagedone(number);
				}
				else
					sql.setretries(number);//ץȡʧ��,����һ��ʧ�ܴ���
				curl_global_cleanup();
			}
			if (file != nullptr)fclose(file);
		}
		Sleep(T->READCFG::GetCFG().WorkTimeSec * 1000);
	}
	return 0;
}

size_t SERVER::analysecallback(char* ptr, size_t size, size_t nmemb, std::string* html)
{
	size_t real_size = nmemb * size;
	html->append(ptr, real_size);
	return real_size;
}

SERVER::image_d SERVER::GetUrl(std::string html)
{
	image_d d;
	d.succeed = false;
	std::string::size_type m = html.find("\">Download PNG");
	if (m != std::string::npos) //������png�ļ�
	{
		html.erase(html.begin() + m, html.end());
		for (int i = html.size(); i; i--)
		{
			if (html.at(i - 1) == '\"')
			{
				html.erase(html.begin(), html.begin() + i);
				break;
			}
		}
		d.url = html;
		for (int i = html.size(); i; i--)
		{
			if (html.at(i - 1) == '/')
			{
				html.erase(html.begin(), html.begin() + i);
				break;
			}
		}
		d.name = html;
		d.succeed = true;
	}
	else
	{
		m = html.find("\">Download larger version");
		if (m != std::string::npos)//������jpg�ļ�
		{
			html.erase(html.begin() + m, html.end());
			for (int i = html.size(); i; i--)
			{
				if (html.at(i - 1) == '\"')
				{
					html.erase(html.begin(), html.begin() + i);
					break;
				}
			}
			d.url = html;
			for (int i = html.size(); i; i--)
			{
				if (html.at(i - 1) == '/')
				{
					html.erase(html.begin(), html.begin() + i);
					break;
				}
			}

			d.name = html;
			d.succeed = true;
		}
		else
		{
			//��û��jpgҲû��png????
			//������ܻ���Ҫ���¼,�ֻ��߱�ɾ����
		}
	}

	return d;
}
