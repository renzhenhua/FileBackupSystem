#pragma once

#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#define BUFSIZE 65536  //缓冲区大小

using namespace std;

struct INDEX1  //索引文件结构，用来记录打包的文件信息
{
	char FileName[MAX_PATH];  //文件名
	unsigned int Size;  //文件大小
};

class Unpack
{
private:
	char m_szFileName[MAX_PATH];  //打包文件路径
	char m_szIndexName[MAX_PATH];  //索引文件路径
	char m_szDirName[MAX_PATH];  //解包后存放的文件夹路径
	HANDLE m_hFile;  //打包文件句柄
	HANDLE m_hFileIndex;  //索引文件句柄
	INDEX1 m_Index;
public:
	bool Init();//解包前的初始化，与用户进行交互
				//索引文件要和打包文件在同一目录，且文件名相同
	bool UnpackFile();// 开始解包
};

