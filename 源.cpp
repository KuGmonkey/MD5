#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>
using namespace std;

//压缩函数各轮的各步中A寄存器循环左移的位数
const unsigned s[64] =
{
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

//常数表T，用无符号整型表示
const unsigned T[64] =
{
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

//基本逻辑函数
#define F(b,c,d) (( b & c ) | (( ~b ) & ( d )))
#define G(b,c,d) (( b & d ) | ( c & ( ~d )))
#define H(b,c,d) ( b ^ c ^ d )
#define I(b,c,d) ( c ^ ( b | ( ~d )))

//x循环左移n位
#define Shift_left(x,n) (( x << n ) | ( x >> ( 32 - n )))

//900150983cd24fb0d6963f7d28e17f72
//将32位整型转化为16进制字符串用于输出
string Int_HexStr(int input)
{
	const char alpht[] = "0123456789abcdef";

	//生成的最终的16进制字符串
	string hex_String = "";

	for (int i = 0; i < 4; i++)
	{
		//每次取input的低8位（因为缓冲区寄存器是以小端方式存储的，所以从低位开始取，转换成字符串放在前头）
		unsigned hex_Num = (input >> (i * 8)) & 0xff;

		//将低八位转换成两个字符，拼接在临时字符串中
		string temp = "";
		temp += alpht[hex_Num / 16];
		temp += alpht[hex_Num % 16];

		//接在结果后边
		hex_String += temp;
	}
	return hex_String;
}

//MD5压缩函数
string MD5(string m)
{
	//缓冲区初始值
	unsigned int A = 0x67452301;
	unsigned int B = 0xefcdab89;
	unsigned int C = 0x98badcfe;
	unsigned int D = 0x10325476;

	//待压缩消息的字节长度
	int len_byte = m.length();
	//按填充后的长度进行分组，共分为group_num组(填充：无论如何必须填充512bit，所以直接group_num加一即可，还需要有64位存储消息长度)
	int group_num = ((len_byte + 8) / 64) + 1;
	//32位无符号整型数组存储各消息分组的32位数字形式
	unsigned int* m_byte = new unsigned int[group_num * 16];
	memset(m_byte, 0, sizeof(unsigned int) * group_num * 16);
	//填入消息
	for (int i = 0; i < len_byte; i++)
	{
		//消息的每四个字符正好是32位，存储为m_byte的一个元素
		m_byte[i / 4] |= m[i] << ((i % 4) * 8);
	}
	//填充位
	m_byte[len_byte >> 2] |= 0x80 << ((len_byte % 4) * 8);
	//填充64bit的消息长度
	m_byte[group_num * 16 - 2] = len_byte * 8;

	//缓冲区的四个寄存器
	unsigned int a, b, c, d;
	for (int i = 0; i < group_num; i++)
	{
		//用上一次的中间结果为缓冲区赋值
		a = A;
		b = B;
		c = C;
		d = D;
		//每一步的基本逻辑函数的结果
		unsigned int g;
		//用于X[k]寻址，即明文的某一分组的某一个字的寻址
		int k;

		for (int j = 0; j < 64; j++)
		{
			if (j < 16)
			{
				g = F(b, c, d);
				k = j;
			}
			else if (j >= 16 && j < 32)
			{
				g = G(b, c, d);
				k = (1 + 5 * j) % 16;
			}
			else if (j >= 32 && j < 48)
			{
				g = H(b, c, d);
				k = (5 + 3 * j) % 16;
			}
			else if (j >= 48 && j < 64)
			{
				g = I(b, c, d);
				k = (7 * j) % 16;
			}

			unsigned tempd = d;
			d = c;
			c = b;
			b = b + Shift_left(a + g + m_byte[i * 16 + k] + T[j], s[j]);
			a = tempd;
		}
		A = a + A;
		B = b + B;
		C = c + C;
		D = d + D;
	}
	return Int_HexStr(A) + Int_HexStr(B) + Int_HexStr(C) + Int_HexStr(D);
}

//计算两个16进制字符串所代表的二进制串的不同的位数，用于检验雪崩效应
int diff_bit_num(string Str1, string Str2)
{
	int diff_Bit = 0;
	for (int part = 0; part < 8; part++)
	{
		string Str1_temp = Str1.substr(0 + 4 * part, 4);
		char* end_1;
		long Str1_Int = static_cast<long>(strtol(Str1_temp.c_str(), &end_1, 16));
		string Str2_temp = Str2.substr(0 + 4 * part, 4);
		char* end_2;
		long Str2_Int = static_cast<long>(strtol(Str2_temp.c_str(), &end_2, 16));
		for (int round = 0; round < 16; round++)
		{
			if (Str1_Int % 2 != Str2_Int % 2)
			{
				diff_Bit++;
			}
			Str1_Int /= 2;
			Str2_Int /= 2;
		}
	}
	return diff_Bit;
}

int main()
{
	cout << "***** MD5 *****" << endl;
	while (1)
	{
		cout << "请选择你的操作:[0:计算字符串的MD5; 1:计算文件消息的MD5; 2:测试雪崩效应; q:退出程序]" << endl;
		char flag;
		cin >> flag;
		if (flag == '0')
		{
			cout << "请输入明文: ";
			string m = ""; cin >> m;
			cout << "明文的MD5值: " << MD5(m) << endl;
		}
		else if (flag == '1')
		{
			cout << "请输入文件路径: ";
			string filepath = ""; cin >> filepath;
			ifstream file(filepath.data());
			ostringstream buf;
			char c;
			while (buf && file.get(c))
				buf.put(c);
			string input = buf.str();
			cout << "消息: " << input << endl << "消息的MD5值: " << MD5(input) << endl;
		}
		else if (flag == '2')
		{
			cout << "测试的原始消息: ";
			//string m = ""; cin >> m;
			string m = "abcdefgh"; cout << m << endl;
			string origin = MD5(m);
			cout << "原始消息的MD5值: " << origin << endl;
			cout << endl;

			int change[8];
			string change1 = "bbcdefgh"; string change2 = "accdefgh"; string change3 = "abddefgh"; string change4 = "abceefgh";
			string change5 = "abcdffgh"; string change6 = "abcdeggh"; string change7 = "abcdefhh"; string change8 = "abcdefgi";
			change[0] = diff_bit_num(MD5(change1), origin); change[1] = diff_bit_num(MD5(change2), origin);
			change[2] = diff_bit_num(MD5(change3), origin); change[3] = diff_bit_num(MD5(change4), origin);
			change[4] = diff_bit_num(MD5(change5), origin); change[5] = diff_bit_num(MD5(change6), origin);
			change[6] = diff_bit_num(MD5(change7), origin); change[7] = diff_bit_num(MD5(change8), origin);

			int sum = 0;
			for (int i = 0; i < 8; i++)
				sum += change[i];
			double avg = double(sum) / 8;

			cout << "第1次测试" << endl << "将消息改变为: " << change1 << endl;
			cout << "改变1的MD5值: " << MD5(change1) << endl;
			cout << "MD5值改变的位数: " << change[0] << endl;
			cout << endl;

			cout << "第2次测试" << endl << "将消息改变为: " << change2 << endl;
			cout << "改变2的MD5值: " << MD5(change2) << endl;
			cout << "MD5值改变的位数: " << change[1] << endl;
			cout << endl;

			cout << "第3次测试" << endl << "将消息改变为: " << change3 << endl;
			cout << "改变3的MD5值: " << MD5(change3) << endl;
			cout << "MD5值改变的位数: " << change[2] << endl;
			cout << endl;

			cout << "第4次测试" << endl << "将消息改变为: " << change4 << endl;
			cout << "改变4的MD5值: " << MD5(change4) << endl;
			cout << "MD5值改变的位数: " << change[3] << endl;
			cout << endl;

			cout << "第5次测试" << endl << "将消息改变为: " << change5 << endl;
			cout << "改变5的MD5值: " << MD5(change5) << endl;
			cout << "MD5值改变的位数: " << change[4] << endl;
			cout << endl;

			cout << "第6次测试" << endl << "将消息改变为: " << change6 << endl;
			cout << "改变6的MD5值: " << MD5(change6) << endl;
			cout << "MD5值改变的位数: " << change[5] << endl;
			cout << endl;

			cout << "第7次测试" << endl << "将消息改变为: " << change7 << endl;
			cout << "改变7的MD5值: " << MD5(change7) << endl;
			cout << "MD5值改变的位数: " << change[6] << endl;
			cout << endl;

			cout << "第8次测试" << endl << "将消息改变为: " << change8 << endl;
			cout << "改变8的MD5值: " << MD5(change8) << endl;
			cout << "MD5值改变的位数: " << change[7] << endl;
			cout << endl;

			cout << "平均改变位数: " << avg << endl;
		}
		else if (flag == 'q')
		{
			cout << "再见！" << endl;
			break;
		}
		else
		{
			cout << "请输入正确指令！" << endl;
			continue;
		}
	}

	system("pause");
	return 0;
}