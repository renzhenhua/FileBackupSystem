#pragma warning (disable: 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ͳ���ַ�Ƶ�ȵ���ʱ���
typedef struct {
	unsigned char uchar;			// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long frequency;		// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
} CharactersFrequency;

// �����������
typedef struct {
	unsigned char uchar;				// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long frequency;			// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
	char *code;						// �ַ���Ӧ�Ĺ��������루��̬����洢�ռ䣩
	int parent, lchild, rchild;		// ����˫�׺����Һ���
} Huffman, *HuffmanTree;

/*
��������������õ����ַ��Ĺ��������롣
*/
//�ҵ���С�ʹ�С���������
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
	//�������ѡ��
	huffman_tree[*a].parent = 1;

	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huffman_tree[i].parent == 0 && huffman_tree[i].frequency < min)
		{
			min = huffman_tree[i].frequency;
			*b = i;
		}
}

//������������
void CreateTree(Huffman *huffman_tree, unsigned int char_kind, unsigned int number_node)
{
	unsigned int i;
	int a, b;
	for (i = char_kind; i < number_node; ++i)
	{
		//ѡ����С���������
		Select(huffman_tree, i, &a, &b);
		huffman_tree[a].parent = huffman_tree[b].parent = i;
		huffman_tree[i].lchild = a;
		huffman_tree[i].rchild = b;
		huffman_tree[i].frequency = huffman_tree[a].frequency + huffman_tree[b].frequency;
	}
}

//���ɹ���������
void HuffmanCode(Huffman *huffman_tree, unsigned int char_kind)
{
	unsigned int i;
	int cur, next, index;
	//�ݴ���룬���256��Ҷ�ӣ����볤�Ȳ�����255
	char *code_temporarily = (char *)malloc(256 * sizeof(char));
	code_temporarily[256 - 1] = '\0';

	for (i = 0; i < char_kind; ++i)
	{
		//������ʱ�ռ�������ʼ��
		index = 256 - 1;

		//��Ҷ�����������������
		for (cur = i, next = huffman_tree[i].parent; next != 0; cur = next, next = huffman_tree[next].parent)
		{
			if (huffman_tree[next].lchild == cur)
			{
				//��0��
				code_temporarily[--index] = '0';
			}
			else
			{
				//�ҡ�1��
				code_temporarily[--index] = '1';
			}
		}
		//Ϊ��i���ַ����붯̬����洢�ռ� 
		huffman_tree[i].code = (char *)malloc((256 - index) * sizeof(char));
		//���򱣴���뵽�������Ӧ����
		strcpy(huffman_tree[i].code, &code_temporarily[index]);
	}
	//�ͷű�����ʱ�ռ�
	free(code_temporarily);
}
/*
���չ��������뽫�ļ�A����ΪHuffman�����ļ�B��
*/
// ѹ������
int Compress(char *input_file_name, char *output_file_name)
{
	unsigned int i, j;
	//�ַ�����
	unsigned int char_kind;
	//�ݴ�8bits�ַ�
	unsigned char temporary_char;
	unsigned long file_length = 0;
	FILE *input_file, *output_file;
	CharactersFrequency temporary_node;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//������뻺����
	char code_the_buffer[256] = "\0";
	unsigned int code_length;

	/*
	** ��̬����256����㣬�ݴ��ַ�Ƶ�ȣ�
	** ͳ�Ʋ������������������ͷ�
	*/
	CharactersFrequency *temporary_char_frequency = (CharactersFrequency *)malloc(256 * sizeof(CharactersFrequency));

	//��ʼ���ݴ���
	for (i = 0; i < 256; ++i)
	{
		temporary_char_frequency[i].frequency = 0;
		//�����256���±���256���ַ���Ӧ
		temporary_char_frequency[i].uchar = (unsigned char)i;
	}

	//�����ļ�����ȡ�ַ�Ƶ��
	input_file = fopen(input_file_name, "rb");
	//�ж������ļ��Ƿ����
	if (input_file == NULL)
	{
		return -1;
	}

	//����һ���ַ�
	fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	while (!feof(input_file))
	{
		//ͳ���±��Ӧ�ַ���Ȩ�أ����������������ʿ���ͳ���ַ�Ƶ��
		++temporary_char_frequency[temporary_char].frequency;
		++file_length;
		//����һ���ַ�
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	}
	fclose(input_file);

	// ���򣬽�Ƶ��Ϊ��ķ�����޳�
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

	// ͳ��ʵ�ʵ��ַ����ࣨ���ִ�����Ϊ0��
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
		//��ѹ�������ɵ��ļ�
		output_file = fopen(output_file_name, "wb");
		//д���ַ�����
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		//д��Ψһ���ַ�
		fwrite((char *)&temporary_char_frequency[0].uchar, sizeof(unsigned char), 1, output_file);
		//д���ַ�Ƶ�ȣ�Ҳ�����ļ�����
		fwrite((char *)&temporary_char_frequency[0].frequency, sizeof(unsigned long), 1, output_file);
		free(temporary_char_frequency);
		fclose(output_file);
	}
	else
	{
		//�����ַ������������㽨������������������
		number_node = 2 * char_kind - 1;
		//��̬������������������
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));

		//��ʼ��ǰchar_kind�����
		for (i = 0; i < char_kind; ++i)
		{
			//���ݴ�����ַ���Ƶ�ȿ����������
			huffman_tree[i].uchar = temporary_char_frequency[i].uchar;
			huffman_tree[i].frequency = temporary_char_frequency[i].frequency;
			huffman_tree[i].parent = 0;
		}
		//�ͷ��ַ�Ƶ��ͳ�Ƶ��ݴ���
		free(temporary_char_frequency);

		//��ʼ����number_node-char_kind�����
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//������������
		CreateTree(huffman_tree, char_kind, number_node);

		//���ɹ���������
		HuffmanCode(huffman_tree, char_kind);

		//д���ַ�����ӦȨ�أ�����ѹʱ�ؽ���������
		//��ѹ�������ɵ��ļ�
		output_file = fopen(output_file_name, "wb");
		//д���ַ�����
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		for (i = 0; i < char_kind; ++i)
		{
			//д���ַ��������򣬶�����˳�򲻱䣩
			fwrite((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, output_file);
			//д���ַ���ӦȨ��
			fwrite((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, output_file);
		}

		//�������ַ���Ȩ����Ϣ����д���ļ����Ⱥ��ַ�����
		//д���ļ�����
		fwrite((char *)&file_length, sizeof(unsigned long), 1, output_file);
		//�Զ�������ʽ�򿪴�ѹ�����ļ�
		input_file = fopen(input_file_name, "rb");
		//ÿ�ζ�ȡ8bits
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		while (!feof(input_file))
		{
			//ƥ���ַ���Ӧ����
			for (i = 0; i < char_kind; ++i)
			{
				if (temporary_char == huffman_tree[i].uchar)
				{
					strcat(code_the_buffer, huffman_tree[i].code);
				}
			}
			//��8λ��һ���ֽڳ��ȣ�Ϊ����Ԫ
			while (strlen(code_the_buffer) >= 8)
			{
				//����ַ��ݴ�ռ䣬��Ϊ�ݴ��ַ���Ӧ����
				temporary_char = '\0';
				for (i = 0; i < 8; ++i)
				{
					//����һλ��Ϊ��һ��bit�ڳ�λ��
					temporary_char <<= 1;
					if (code_the_buffer[i] == '1')
					{
						//������Ϊ"1"��ͨ���������������ӵ��ֽڵ����λ
						temporary_char |= 1;
					}
				}
				//���ֽڶ�Ӧ��������ļ�
				fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
				//���뻺��ȥ���Ѵ����ǰ��λ
				strcpy(code_the_buffer, code_the_buffer + 8);
			}
			//ÿ�ζ�ȡ8bits
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		}
		//���������8bits����
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
			//�������ֶδ�β���Ƶ��ֽڵĸ�λ
			temporary_char <<= 8 - code_length;
			//�������һ���ֽ�
			fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
		}

		// �ر��ļ�
		fclose(input_file);
		fclose(output_file);

		// �ͷ��ڴ�
		for (i = 0; i < char_kind; ++i)
		{
			free(huffman_tree[i].code);
		}
		free(huffman_tree);
	}
}
/*
���룺���ļ�B�������룬�õ��ļ�C
*/
int Uncompress(char *input_file_name, char *output_file_name)
{
	unsigned int i;
	unsigned long file_length;
	//�����ļ�д�볤��
	unsigned long file_write_length = 0;
	FILE *input_file, *output_file;
	//�洢�ַ�����
	unsigned int char_kind;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//�ݴ�8bits����
	unsigned char temporary_char;
	//������ڵ���������ƥ�����ʹ��
	unsigned int root;

	//�Զ����Ʒ�ʽ��ѹ���ļ�
	input_file = fopen(input_file_name, "rb");

	if (input_file == NULL)
	{
		return -1;
	}

	//��ȡѹ���ļ�ǰ�˵��ַ�����Ӧ���룬�����ؽ���������
	//��ȡ�ַ�������
	fread((char *)&char_kind, sizeof(unsigned int), 1, input_file);
	if (char_kind == 1)
	{
		//��ȡΨһ���ַ�
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		//��ȡ�ļ�����
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//��ѹ�������ɵ��ļ�
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
		//�����ַ������������㽨������������������ 
		number_node = 2 * char_kind - 1;
		//��̬��������������ռ�
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));
		//��ȡ�ַ�����ӦȨ�أ�������������ڵ�
		for (i = 0; i < char_kind; ++i)
		{
			//�����ַ�
			fread((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, input_file);
			//�����ַ���ӦȨ��
			fread((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, input_file);
			huffman_tree[i].parent = 0;
		}
		//��ʼ����number_node-char_kins������parent
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//�ؽ�������������ѹ��ʱ��һ�£�
		CreateTree(huffman_tree, char_kind, number_node);

		//�����ַ���Ȩ����Ϣ�������Ŷ�ȡ�ļ����Ⱥͱ��룬���н���
		//�����ļ�����
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//��ѹ�������ɵ��ļ�
		output_file = fopen(output_file_name, "wb");
		root = number_node - 1;
		while (1)
		{
			//��ȡһ���ַ����ȵı��루8λ��
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);

			//�����ȡ��һ���ַ����ȵı���
			for (i = 0; i < 8; ++i)
			{
				//�ɸ�����ֱ��Ҷ�ڵ�����ƥ������Ӧ�ַ�
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
					//�����ļ����ȣ������ڲ�ѭ��
					if (file_write_length == file_length)
					{
						break;
					}
					//��λΪ��������ƥ����һ���ַ�
					root = number_node - 1;
				}
				//�����뻺�����һλ�Ƶ����λ����ƥ��
				temporary_char <<= 1;
			}
			//�����ļ����ȣ��������ѭ��
			if (file_write_length == file_length)
			{
				break;
			}
		}

		//�ر��ļ�
		fclose(input_file);
		fclose(output_file);

		//�ͷ��ڴ�
		free(huffman_tree);
	}
}

int main()
{
	while (1)
	{
		int n, flag = 0;
		char name[2222], names[2222], input_file_name[2222], output_file_name[2222];
		printf("1��ѹ��\n2����ѹ��\n���������˳�,��ѡ���ܣ�");
		scanf("%d", &n);
		getchar();
		switch (n)
		{
		case 1:
			printf("�����ļ�����Ŀ¼:\n");//���ӻ�ʱ��������ѡ��
			scanf("%s", input_file_name);
			printf("�����ļ�����Ŀ¼:\n");
			scanf("%s", output_file_name);
			getchar();
			printf("�����뽫Ҫ���в������ļ�����");
			gets_s(name);
			strcat(input_file_name, name);
			printf("��������в�����������ļ�����");
			gets_s(names);
			strcat(output_file_name, names);
			printf("���ڽ���ѹ��\n");

			flag = Compress(input_file_name, output_file_name);
			break;
		case 2:
			printf("�����ļ�����Ŀ¼:\n");//���ӻ�ʱ��������ѡ��
			scanf("%s", input_file_name);
			printf("�����ļ�����Ŀ¼:\n");
			scanf("%s", output_file_name);
			getchar();
			printf("�����뽫Ҫ���в������ļ�����");
			gets_s(name);
			strcat(input_file_name, name);
			printf("��������в�����������ļ�����");
			gets_s(names);
			strcat(output_file_name, names);
			printf("���ڽ��н�ѹ��\n");

			flag = Uncompress(input_file_name, output_file_name);
			break;
		default:
			return 0;
		}

		if (flag == -1)
		{
			printf("�ļ�\"%s\"������!\n", input_file_name);
		}
		else
		{
			printf("���!\n\n");
		}
	}
}
