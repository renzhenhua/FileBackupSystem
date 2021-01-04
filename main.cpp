#pragma warning (disable: 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define gets_s gets
// 统计字符频度的临时结点
typedef struct {
	unsigned char uchar;			// 以8bits为单元的无符号字符
	unsigned long frequency;		// 每类（以二进制编码区分）字符出现频度
} CharactersFrequency;

// 哈夫曼树结点
typedef struct {
	unsigned char uchar;				// 以8bits为单元的无符号字符
	unsigned long frequency;			// 每类（以二进制编码区分）字符出现频度
	char *code;						// 字符对应的哈夫曼编码（动态分配存储空间）
	int parent, lchild, rchild;		// 定义双亲和左右孩子
} Huffman, *HuffmanTree;

/*
构造哈夫曼树，得到各字符的哈夫曼编码。
*/
//找到最小和次小的两个结点
void Select(Huffman *huffman_tree, unsigned int n, int *a, int *b)
{
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huffman_tree[i].parent == 0 && huffman_tree[i].frequency < min)
		{
			min = huffman_tree[i].frequency;
			*a = i;
		}
	//做标记已选中
	huffman_tree[*a].parent = 1;

	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huffman_tree[i].parent == 0 && huffman_tree[i].frequency < min)
		{
			min = huffman_tree[i].frequency;
			*b = i;
		}
}

//建立哈夫曼树
void CreateTree(Huffman *huffman_tree, unsigned int char_kind, unsigned int number_node)
{
	unsigned int i;
	int a, b;
	for (i = char_kind; i < number_node; ++i)
	{
		//选择最小的两个结点
		Select(huffman_tree, i, &a, &b);
		huffman_tree[a].parent = huffman_tree[b].parent = i;
		huffman_tree[i].lchild = a;
		huffman_tree[i].rchild = b;
		huffman_tree[i].frequency = huffman_tree[a].frequency + huffman_tree[b].frequency;
	}
}

//生成哈夫曼编码
void HuffmanCode(Huffman *huffman_tree, unsigned int char_kind)
{
	unsigned int i;
	int cur, next, index;
	//暂存编码，最多256个叶子，编码长度不超过255
	char *code_temporarily = (char *)malloc(256 * sizeof(char));
	code_temporarily[256 - 1] = '\0';

	for (i = 0; i < char_kind; ++i)
	{
		//编码临时空间索引初始化
		index = 256 - 1;

		//从叶子向根反向遍历求编码
		for (cur = i, next = huffman_tree[i].parent; next != 0; cur = next, next = huffman_tree[next].parent)
		{
			if (huffman_tree[next].lchild == cur)
			{
				//左‘0’
				code_temporarily[--index] = '0';
			}
			else
			{
				//右‘1’
				code_temporarily[--index] = '1';
			}
		}
		//为第i个字符编码动态分配存储空间 
		huffman_tree[i].code = (char *)malloc((256 - index) * sizeof(char));
		//正向保存编码到树结点相应域中
		strcpy(huffman_tree[i].code, &code_temporarily[index]);
	}
	//释放编码临时空间
	free(code_temporarily);
}
/*
按照哈夫曼编码将文件A翻译为Huffman编码文件B。
*/
// 压缩函数
int Compress(char *input_file_name, char *output_file_name)
{
	unsigned int i, j;
	//字符种类
	unsigned int char_kind;
	//暂存8bits字符
	unsigned char temporary_char;
	unsigned long file_length = 0;
	FILE *input_file, *output_file;
	CharactersFrequency temporary_node;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//待存编码缓冲区
	char code_the_buffer[256] = "\0";
	unsigned int code_length;

	/*
	** 动态分配256个结点，暂存字符频度，
	** 统计并拷贝到树结点后立即释放
	*/
	CharactersFrequency *temporary_char_frequency = (CharactersFrequency *)malloc(256 * sizeof(CharactersFrequency));

	//初始化暂存结点
	for (i = 0; i < 256; ++i)
	{
		temporary_char_frequency[i].frequency = 0;
		//数组的256个下标与256种字符对应
		temporary_char_frequency[i].uchar = (unsigned char)i;
	}

	//遍历文件，获取字符频度
	input_file = fopen(input_file_name, "rb");
	//判断输入文件是否存在
	if (input_file == NULL)
	{
		return -1;
	}

	//读入一个字符
	fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	while (!feof(input_file))
	{
		//统计下标对应字符的权重，利用数组的随机访问快速统计字符频度
		++temporary_char_frequency[temporary_char].frequency;
		++file_length;
		//读入一个字符
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	}
	fclose(input_file);

	// 排序，将频度为零的放最后，剔除
	for (i = 0; i < 256 - 1; ++i)
	{
		for (j = i + 1; j < 256; ++j)
			if (temporary_char_frequency[i].frequency < temporary_char_frequency[j].frequency)
			{
				temporary_node = temporary_char_frequency[i];
				temporary_char_frequency[i] = temporary_char_frequency[j];
				temporary_char_frequency[j] = temporary_node;
			}
	}

	// 统计实际的字符种类（出现次数不为0）
	for (i = 0; i < 256; ++i)
	{
		if (temporary_char_frequency[i].frequency == 0)
		{
			break;
		}
	}
	char_kind = i;

	if (char_kind == 1)
	{
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		//写入字符种类
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		//写入唯一的字符
		fwrite((char *)&temporary_char_frequency[0].uchar, sizeof(unsigned char), 1, output_file);
		//写入字符频度，也就是文件长度
		fwrite((char *)&temporary_char_frequency[0].frequency, sizeof(unsigned long), 1, output_file);
		free(temporary_char_frequency);
		fclose(output_file);
	}
	else
	{
		//根据字符种类数，计算建立哈夫曼树所需结点数
		number_node = 2 * char_kind - 1;
		//动态建立哈夫曼树所需结点
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));

		//初始化前char_kind个结点
		for (i = 0; i < char_kind; ++i)
		{
			//将暂存结点的字符和频度拷贝到树结点
			huffman_tree[i].uchar = temporary_char_frequency[i].uchar;
			huffman_tree[i].frequency = temporary_char_frequency[i].frequency;
			huffman_tree[i].parent = 0;
		}
		//释放字符频度统计的暂存区
		free(temporary_char_frequency);

		//初始化后number_node-char_kind个结点
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//创建哈夫曼树
		CreateTree(huffman_tree, char_kind, number_node);

		//生成哈夫曼编码
		HuffmanCode(huffman_tree, char_kind);

		//写入字符和相应权重，供解压时重建哈夫曼树
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		//写入字符种类
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		for (i = 0; i < char_kind; ++i)
		{
			//写入字符（已排序，读出后顺序不变）
			fwrite((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, output_file);
			//写入字符对应权重
			fwrite((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, output_file);
		}

		//紧接着字符和权重信息后面写入文件长度和字符编码
		//写入文件长度
		fwrite((char *)&file_length, sizeof(unsigned long), 1, output_file);
		//以二进制形式打开待压缩的文件
		input_file = fopen(input_file_name, "rb");
		//每次读取8bits
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		while (!feof(input_file))
		{
			//匹配字符对应编码
			for (i = 0; i < char_kind; ++i)
			{
				if (temporary_char == huffman_tree[i].uchar)
				{
					strcat(code_the_buffer, huffman_tree[i].code);
				}
			}
			//以8位（一个字节长度）为处理单元
			while (strlen(code_the_buffer) >= 8)
			{
				//清空字符暂存空间，改为暂存字符对应编码
				temporary_char = '\0';
				for (i = 0; i < 8; ++i)
				{
					//左移一位，为下一个bit腾出位置
					temporary_char <<= 1;
					if (code_the_buffer[i] == '1')
					{
						//当编码为"1"，通过或操作符将其添加到字节的最低位
						temporary_char |= 1;
					}
				}
				//将字节对应编码存入文件
				fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
				//编码缓存去除已处理的前八位
				strcpy(code_the_buffer, code_the_buffer + 8);
			}
			//每次读取8bits
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		}
		//处理最后不足8bits编码
		code_length = strlen(code_the_buffer);
		if (code_length > 0)
		{
			temporary_char = '\0';
			for (i = 0; i < code_length; ++i)
			{
				temporary_char <<= 1;
				if (code_the_buffer[i] == '1')
				{
					temporary_char |= 1;
				}
			}
			//将编码字段从尾部移到字节的高位
			temporary_char <<= 8 - code_length;
			//存入最后一个字节
			fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
		}

		// 关闭文件
		fclose(input_file);
		fclose(output_file);

		// 释放内存
		for (i = 0; i < char_kind; ++i)
		{
			free(huffman_tree[i].code);
		}
		free(huffman_tree);
	}
}
/*
译码：对文件B进行译码，得到文件C
*/
int Uncompress(char *input_file_name, char *output_file_name)
{
	unsigned int i;
	unsigned long file_length;
	//控制文件写入长度
	unsigned long file_write_length = 0;
	FILE *input_file, *output_file;
	//存储字符种类
	unsigned int char_kind;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//暂存8bits编码
	unsigned char temporary_char;
	//保存根节点索引，供匹配编码使用
	unsigned int root;

	//以二进制方式打开压缩文件
	input_file = fopen(input_file_name, "rb");

	if (input_file == NULL)
	{
		return -1;
	}

	//读取压缩文件前端的字符及对应编码，用于重建哈夫曼树
	//读取字符种类数
	fread((char *)&char_kind, sizeof(unsigned int), 1, input_file);
	if (char_kind == 1)
	{
		//读取唯一的字符
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		//读取文件长度
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		while (file_length--)
		{
			fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
		}
		fclose(input_file);
		fclose(output_file);
	}
	else
	{
		//根据字符种类数，计算建立哈夫曼树所需结点数 
		number_node = 2 * char_kind - 1;
		//动态分配哈夫曼树结点空间
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));
		//读取字符及对应权重，存入哈夫曼树节点
		for (i = 0; i < char_kind; ++i)
		{
			//读入字符
			fread((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, input_file);
			//读入字符对应权重
			fread((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, input_file);
			huffman_tree[i].parent = 0;
		}
		//初始化后number_node-char_kins个结点的parent
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//重建哈夫曼树（与压缩时的一致）
		CreateTree(huffman_tree, char_kind, number_node);

		//读完字符和权重信息，紧接着读取文件长度和编码，进行解码
		//读入文件长度
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		root = number_node - 1;
		while (1)
		{
			//读取一个字符长度的编码（8位）
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);

			//处理读取的一个字符长度的编码
			for (i = 0; i < 8; ++i)
			{
				//由根向下直至叶节点正向匹配编码对应字符
				if (temporary_char & 128)
				{
					root = huffman_tree[root].rchild;
				}
				else
				{
					root = huffman_tree[root].lchild;
				}

				if (root < char_kind)
				{
					fwrite((char *)&huffman_tree[root].uchar, sizeof(unsigned char), 1, output_file);
					++file_write_length;
					//控制文件长度，跳出内层循环
					if (file_write_length == file_length)
					{
						break;
					}
					//复位为根索引，匹配下一个字符
					root = number_node - 1;
				}
				//将编码缓存的下一位移到最高位，供匹配
				temporary_char <<= 1;
			}
			//控制文件长度，跳出外层循环
			if (file_write_length == file_length)
			{
				break;
			}
		}
		//关闭文件
		fclose(input_file);
		fclose(output_file);
		//释放内存
		free(huffman_tree);
	}
}

int main()
{
	while (1)
	{
		int n, flag = 0;
		char name[2222]={".KCS0075"}, names[2222]={"ys.tar"}, input_file_name[2222], output_file_name[2222];
		//printf("1、压缩\n2、解压缩\n输入其他退出,请选择功能：");
		scanf("%d", &n);
		getchar();
		switch (n)
		{
		case 1:
			//printf("输入文件输入目录:\n");//可视化时可做窗口选择
			scanf("%s", input_file_name);
			//printf("输入文件保存目录:\n");
			scanf("%s", output_file_name);
			//getchar();
			//printf("请输入将要进行操作的文件名：");
			//gets_s(name);
			strcat(input_file_name, name);
			//printf("请输入进行操作后输出的文件名：");
			//gets_s(names);
			strcat(output_file_name, names);
			//printf("正在进行压缩\n");

			flag = Compress(input_file_name, output_file_name);
			break;
		case 2:
			//printf("输入文件输入目录:\n");//可视化时可做窗口选择
			scanf("%s", input_file_name);
			//printf("输入文件保存目录:\n");
			scanf("%s", output_file_name);
			//getchar();
			//printf("请输入将要进行操作的文件名：");
			strcat(input_file_name, names);
			//printf("请输入进行操作后输出的文件名：");
			strcat(output_file_name, name);
			//printf("正在进行解压缩\n");

			flag = Uncompress(input_file_name, output_file_name);
			break;
		default:
			return 0;
		}

		if (flag == -1)
		{
			printf("文件\"%s\"不存在!\n", input_file_name);
		}
		else
		{
			printf("完成!\n\n");
		}
		break;
	}
}
