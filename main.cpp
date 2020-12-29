#include <iostream>
#include <conio.h>
#include"Pack.h"
#include"Unpack.h"
using namespace std;

int main()
{
	Pack kPack;  //打包类对象
	Unpack kUnpack;  //解包类对象

	char ch;
	while (1)
	{
		system("cls");
		cout << "1.打包" << endl;
		cout << "2.解包(KCS0075)" << endl;
		cout << "0.退出" << endl << endl;
		ch = _getch();
		switch (ch)
		{
		case '1': //打包
			if (kPack.Init())
				kPack.PackFile();
			break;
		case '2': //解包
			if (kUnpack.Init())
				kUnpack.UnpackFile();
			break;
		case '0': //退出
			return 0;
		default:
			cout << "输入错误！" << endl;
		}
		system("pause");
	}
	return 0;
}