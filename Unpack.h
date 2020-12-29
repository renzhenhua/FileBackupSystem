#pragma once

#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#define BUFSIZE 65536  //��������С

using namespace std;

struct INDEX1  //�����ļ��ṹ��������¼������ļ���Ϣ
{
	char FileName[MAX_PATH];  //�ļ���
	unsigned int Size;  //�ļ���С
};

class Unpack
{
private:
	char m_szFileName[MAX_PATH];  //����ļ�·��
	char m_szIndexName[MAX_PATH];  //�����ļ�·��
	char m_szDirName[MAX_PATH];  //������ŵ��ļ���·��
	HANDLE m_hFile;  //����ļ����
	HANDLE m_hFileIndex;  //�����ļ����
	INDEX1 m_Index;
public:
	bool Init();//���ǰ�ĳ�ʼ�������û����н���
				//�����ļ�Ҫ�ʹ���ļ���ͬһĿ¼�����ļ�����ͬ
	bool UnpackFile();// ��ʼ���
};

