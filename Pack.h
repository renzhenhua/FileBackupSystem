#pragma once

#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#define BUFSIZE 65536  //缓冲区大小

using namespace std;

struct INDEX  //索引文件结构，用来记录打包的文件信息
{
	char FileName[MAX_PATH];  //文件名
	unsigned int Size;  //文件大小
};

class Pack
{
private:
	char m_szDirName[MAX_PATH];   //要打包的文件夹的路径
	char m_szSaveName[MAX_PATH];  //打包后存放路径
	char m_szIndexName[MAX_PATH]; //打包后的索引路径
	HANDLE m_hFile;               //打包文件句柄
	HANDLE m_hFindFile;           //遍历文件夹句柄
	HANDLE m_hFileIndex;          //索引文件流
	INDEX m_Index;
public:
	bool Init();      //打包前的初始化，与用户进行交互
	bool PackFile();  //开始打包
};


