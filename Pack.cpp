#include "Pack.h"

bool Pack::Init()
{
	cout << "请输入要打包的文件夹的路径: ";   //这里要输入文件夹的完整路径，如d:/a
	cin >> m_szDirName;
	cout << "请输入要保存的文件名的路径(不用加后缀): ";  //这里的输入包括要生成的打包文件的路径和路径名，如d:/c
	cin >> m_szSaveName;

	strcpy_s(m_szIndexName, m_szSaveName);
	strcat_s(m_szIndexName, ".txt");  //生成索引路径
	strcat_s(m_szSaveName, ".KCS0075");  //给打包文件加路径

	//创建打包文件
	m_hFile = CreateFile(m_szSaveName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)  //如果文件已存在或创建不了
	{
		cout << "File could not create." << endl;
		CloseHandle(m_hFile);
		return 0;
	}
	if (m_szDirName[strlen(m_szDirName) - 1] != '*')  //目录文件要加*
	{
		if (m_szDirName[strlen(m_szDirName) - 1] != '\\')
			strcat_s(m_szDirName, "\\");
		strcat_s(m_szDirName, "*");
	}

	m_hFileIndex = CreateFile(m_szIndexName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFileIndex == INVALID_HANDLE_VALUE)  //如果文件已存在或创建不了
	{
		cout << "Index could not create." << endl;
		CloseHandle(m_hFileIndex);
		return 0;
	}

	return 1;
}


bool Pack::PackFile()
{
	WIN32_FIND_DATA data;
	DWORD buff[BUFSIZE];  //缓冲区
	DWORD dwBytesRead, dwBytesWritten, dwIndexWritten;  //记录读取和写入文件的字节数

	m_hFindFile = FindFirstFile(m_szDirName, &data);  //遍历要打包的文件夹句柄，寻找第一个文件
	if (m_hFindFile == INVALID_HANDLE_VALUE)  //如果没找到
	{
		cout << "File could not find." << endl;
		CloseHandle(m_hFindFile);
		return 0;
	}

	char tag[] = "KCS0075";
	WriteFile(m_hFile, tag, sizeof(tag), &dwBytesWritten, NULL);

	cout << "正在打包..." << endl;
	cout << "---------------------------------------------------" << endl;
	if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&   //判断是否为文件夹
		strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
	{
		char fName[MAX_PATH];  //用来存储要打包的文件夹里各个文件的路径
		strcpy_s(fName, m_szDirName);
		fName[strlen(fName) - 1] = '\0';
		strcat_s(fName, data.cFileName);

		//打开文件
		HANDLE hTmpFile = CreateFile(fName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTmpFile == INVALID_HANDLE_VALUE)
		{
			cout << "File could not find." << endl;
			CloseHandle(hTmpFile);
			return 0;
		}

		strcpy_s(m_Index.FileName, data.cFileName); //文件名
		m_Index.Size = GetFileSize(hTmpFile, NULL);  //文件大小
		cout << data.cFileName << '\t' << m_Index.Size << " B" << endl; //输出正在打包的文件信息

		do
		{//开始写入打包文件
			if (ReadFile(hTmpFile, buff, BUFSIZE, &dwBytesRead, NULL))
				WriteFile(m_hFile, buff, dwBytesRead, &dwBytesWritten, NULL);

		} while (dwBytesRead == BUFSIZE);

		WriteFile(m_hFileIndex, &m_Index, sizeof(m_Index), &dwIndexWritten, NULL);    //将文件信息记入索引文件
		CloseHandle(hTmpFile);
	}

	while (FindNextFile(m_hFindFile, &data))  //遍历文件夹
	{
		if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
		{
			char fName[MAX_PATH];   //用来存储要打包的文件夹里各个文件的路径
			strcpy_s(fName, m_szDirName);
			fName[strlen(fName) - 1] = '\0';
			strcat_s(fName, data.cFileName);

			HANDLE hTmpFile = CreateFile(fName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hTmpFile == INVALID_HANDLE_VALUE)
			{
				cout << "File could not find." << endl;
				CloseHandle(hTmpFile);
				return 0;
			}

			strcpy_s(m_Index.FileName, data.cFileName);  //文件名
			m_Index.Size = GetFileSize(hTmpFile, NULL);  //文件大小
			cout << data.cFileName << '\t' << m_Index.Size << " B" << endl;  //输出正在打包的文件信息

			do
			{
				if (ReadFile(hTmpFile, buff, BUFSIZE, &dwBytesRead, NULL))
					WriteFile(m_hFile, buff, dwBytesRead, &dwBytesWritten, NULL);

			} while (dwBytesRead == BUFSIZE);

			WriteFile(m_hFileIndex, &m_Index, sizeof(m_Index), &dwIndexWritten, NULL);   //将文件信息记入索引文件
			CloseHandle(hTmpFile);
		}
	}
	cout << "---------------------------------------------------" << endl;
	cout << "打包完成!" << endl;

	CloseHandle(m_hFindFile);
	CloseHandle(m_hFile);
	CloseHandle(m_hFileIndex);
	return 1;
}
