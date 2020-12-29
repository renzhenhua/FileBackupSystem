#include "Pack.h"

bool Pack::Init()
{
	cout << "������Ҫ������ļ��е�·��: ";   //����Ҫ�����ļ��е�����·������d:/a
	cin >> m_szDirName;
	cout << "������Ҫ������ļ�����·��(���üӺ�׺): ";  //������������Ҫ���ɵĴ���ļ���·����·��������d:/c
	cin >> m_szSaveName;

	strcpy_s(m_szIndexName, m_szSaveName);
	strcat_s(m_szIndexName, ".txt");  //��������·��
	strcat_s(m_szSaveName, ".KCS0075");  //������ļ���·��

	//��������ļ�
	m_hFile = CreateFile(m_szSaveName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)  //����ļ��Ѵ��ڻ򴴽�����
	{
		cout << "File could not create." << endl;
		CloseHandle(m_hFile);
		return 0;
	}
	if (m_szDirName[strlen(m_szDirName) - 1] != '*')  //Ŀ¼�ļ�Ҫ��*
	{
		if (m_szDirName[strlen(m_szDirName) - 1] != '\\')
			strcat_s(m_szDirName, "\\");
		strcat_s(m_szDirName, "*");
	}

	m_hFileIndex = CreateFile(m_szIndexName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFileIndex == INVALID_HANDLE_VALUE)  //����ļ��Ѵ��ڻ򴴽�����
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
	DWORD buff[BUFSIZE];  //������
	DWORD dwBytesRead, dwBytesWritten, dwIndexWritten;  //��¼��ȡ��д���ļ����ֽ���

	m_hFindFile = FindFirstFile(m_szDirName, &data);  //����Ҫ������ļ��о����Ѱ�ҵ�һ���ļ�
	if (m_hFindFile == INVALID_HANDLE_VALUE)  //���û�ҵ�
	{
		cout << "File could not find." << endl;
		CloseHandle(m_hFindFile);
		return 0;
	}

	char tag[] = "KCS0075";
	WriteFile(m_hFile, tag, sizeof(tag), &dwBytesWritten, NULL);

	cout << "���ڴ��..." << endl;
	cout << "---------------------------------------------------" << endl;
	if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&   //�ж��Ƿ�Ϊ�ļ���
		strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
	{
		char fName[MAX_PATH];  //�����洢Ҫ������ļ���������ļ���·��
		strcpy_s(fName, m_szDirName);
		fName[strlen(fName) - 1] = '\0';
		strcat_s(fName, data.cFileName);

		//���ļ�
		HANDLE hTmpFile = CreateFile(fName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTmpFile == INVALID_HANDLE_VALUE)
		{
			cout << "File could not find." << endl;
			CloseHandle(hTmpFile);
			return 0;
		}

		strcpy_s(m_Index.FileName, data.cFileName); //�ļ���
		m_Index.Size = GetFileSize(hTmpFile, NULL);  //�ļ���С
		cout << data.cFileName << '\t' << m_Index.Size << " B" << endl; //������ڴ�����ļ���Ϣ

		do
		{//��ʼд�����ļ�
			if (ReadFile(hTmpFile, buff, BUFSIZE, &dwBytesRead, NULL))
				WriteFile(m_hFile, buff, dwBytesRead, &dwBytesWritten, NULL);

		} while (dwBytesRead == BUFSIZE);

		WriteFile(m_hFileIndex, &m_Index, sizeof(m_Index), &dwIndexWritten, NULL);    //���ļ���Ϣ���������ļ�
		CloseHandle(hTmpFile);
	}

	while (FindNextFile(m_hFindFile, &data))  //�����ļ���
	{
		if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
		{
			char fName[MAX_PATH];   //�����洢Ҫ������ļ���������ļ���·��
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

			strcpy_s(m_Index.FileName, data.cFileName);  //�ļ���
			m_Index.Size = GetFileSize(hTmpFile, NULL);  //�ļ���С
			cout << data.cFileName << '\t' << m_Index.Size << " B" << endl;  //������ڴ�����ļ���Ϣ

			do
			{
				if (ReadFile(hTmpFile, buff, BUFSIZE, &dwBytesRead, NULL))
					WriteFile(m_hFile, buff, dwBytesRead, &dwBytesWritten, NULL);

			} while (dwBytesRead == BUFSIZE);

			WriteFile(m_hFileIndex, &m_Index, sizeof(m_Index), &dwIndexWritten, NULL);   //���ļ���Ϣ���������ļ�
			CloseHandle(hTmpFile);
		}
	}
	cout << "---------------------------------------------------" << endl;
	cout << "������!" << endl;

	CloseHandle(m_hFindFile);
	CloseHandle(m_hFile);
	CloseHandle(m_hFileIndex);
	return 1;
}
