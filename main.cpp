#include <iostream>
#include <conio.h>
#include"Pack.h"
#include"Unpack.h"
using namespace std;

int main()
{
	Pack kPack;  //��������
	Unpack kUnpack;  //��������

	char ch;
	while (1)
	{
		system("cls");
		cout << "1.���" << endl;
		cout << "2.���(KCS0075)" << endl;
		cout << "0.�˳�" << endl << endl;
		ch = _getch();
		switch (ch)
		{
		case '1': //���
			if (kPack.Init())
				kPack.PackFile();
			break;
		case '2': //���
			if (kUnpack.Init())
				kUnpack.UnpackFile();
			break;
		case '0': //�˳�
			return 0;
		default:
			cout << "�������" << endl;
		}
		system("pause");
	}
	return 0;
}