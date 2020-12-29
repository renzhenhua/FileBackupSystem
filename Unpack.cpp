#include "Unpack.h"

bool Unpack::Init()
{
	cout << "请输入要解包的文件路径: ";
	cin >> m_szFileName;

	//打开打包文件
	m_hFile = CreateFile(m_szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		cout << "File could not open." << endl;
		CloseHandle(m_hFile);
		return 0;
	}

	char tag[] = "KCS0075";
	char ch[8];
	DWORD dwBytesRead;
	ReadFile(m_hFile, ch, sizeof(ch), &dwBytesRead, NULL);
	if (strcmp(tag, ch) != 0)  //判断是否为打包文件
	{
		cout << "此文件不能解包" << endl;
		CloseHandle(m_hFile);
		return 0;
	}

	strcpy_s(m_szIndexName, m_szFileName);
	m_szIndexName[strlen(m_szIndexName) - 8] = '\0';
	strcat_s(m_szIndexName, ".txt");   //获取索引文件路径

	//打开索引文件，这里索引文件要和打包文件在同一目录，且文件名相同
	m_hFileIndex = CreateFile(m_szIndexName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFileIndex == INVALID_HANDLE_VALUE)
	{
		cout << "Index could not open." << endl;
		CloseHandle(m_hFileIndex);
		return 0;
	}

	cout << "请输入要解包到的文件夹的路径: ";
	cin >> m_szDirName;
	CreateDirectory(m_szDirName, NULL); //创建目录

	return 1;
}

bool Unpack::UnpackFile()
{
	DWORD buff[BUFSIZE];  //缓冲区
	DWORD dwBytesRead, dwBytesWritten, dwIndexRead;  //记录读取和写入文件的字节数

	cout << "正在解包..." << endl;
	cout << "--------------------------------------" << endl;
	while (ReadFile(m_hFileIndex, &m_Index, sizeof(m_Index), &dwIndexRead, NULL))  //开始读取索引文件
	{
		if (dwIndexRead <= 0)
			break;

		char fName[MAX_PATH];  //用来存储解包后的各个文件的路径
		strcpy_s(fName, m_szDirName);
		strcat_s(fName, "\\");
		strcat_s(fName, m_Index.FileName);

		//创建解包后的各个文件
		HANDLE hTmpFile = CreateFile(fName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTmpFile == INVALID_HANDLE_VALUE)
		{
			cout << "File could not create." << endl;
			CloseHandle(hTmpFile);
			return 0;
		}
		cout << m_Index.FileName << '\t' << m_Index.Size << " B" << endl;  //输出解包后各个文件的信息
		unsigned int len;  //用来存储每次读取文件的字节数
		do
		{//开始将打包文件里的内容写入解包后的各个文件
			len = m_Index.Size < BUFSIZE ? m_Index.Size : BUFSIZE;
			if (ReadFile(m_hFile, buff, len, &dwBytesRead, NULL))
			{
				WriteFile(hTmpFile, buff, dwBytesRead, &dwBytesWritten, NULL);
				m_Index.Size -= len;
			}

		} while (m_Index.Size > 0);
		CloseHandle(hTmpFile);

	}
	cout << "--------------------------------------" << endl;
	cout << "解包完成！" << endl;

	CloseHandle(m_hFile);
	CloseHandle(m_hFileIndex);
	return 1;
}