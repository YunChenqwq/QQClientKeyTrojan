#pragma once
#include<iostream>
#include<stdarg.h>

using namespace std;

const int BUF_SIZE = 4096;
//ʵ�������
class LogInfo
{
public:
	LogInfo() {};
	~LogInfo() {};


	void logInfo(const char* szFormat, ...)
	{
		char szBuf[BUF_SIZE] = {};
		va_list args;                            //��һ��
		va_start(args, szFormat);                 //�ڶ���
		//_vsnprintf(szBuf, BUF_SIZE, szFormat, args);    //������
		-_vsnprintf_s(szBuf, BUF_SIZE, szFormat, args);
		va_end(args);                            //���Ĳ�

		//������ʵ�������ʽ
		std::cout << szBuf << endl;
		return;
	}
};

