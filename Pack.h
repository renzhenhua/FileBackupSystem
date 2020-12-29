#pragma once

#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#define BUFSIZE 65536  //��������С

using namespace std;

struct INDEX  //�����ļ��ṹ��������¼������ļ���Ϣ
{
	char FileName[MAX_PATH];  //�ļ���
	unsigned int Size;  //�ļ���С
};

class Pack
{
private:
	char m_szDirName[MAX_PATH];   //Ҫ������ļ��е�·��
	char m_szSaveName[MAX_PATH];  //�������·��
	char m_szIndexName[MAX_PATH]; //����������·��
	HANDLE m_hFile;               //����ļ����
	HANDLE m_hFindFile;           //�����ļ��о��
	HANDLE m_hFileIndex;          //�����ļ���
	INDEX m_Index;
public:
	bool Init();      //���ǰ�ĳ�ʼ�������û����н���
	bool PackFile();  //��ʼ���
};


