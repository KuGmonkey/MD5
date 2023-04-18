#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>
using namespace std;

//ѹ���������ֵĸ�����A�Ĵ���ѭ�����Ƶ�λ��
const unsigned s[64] =
{
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

//������T�����޷������ͱ�ʾ
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

//�����߼�����
#define F(b,c,d) (( b & c ) | (( ~b ) & ( d )))
#define G(b,c,d) (( b & d ) | ( c & ( ~d )))
#define H(b,c,d) ( b ^ c ^ d )
#define I(b,c,d) ( c ^ ( b | ( ~d )))

//xѭ������nλ
#define Shift_left(x,n) (( x << n ) | ( x >> ( 32 - n )))

//900150983cd24fb0d6963f7d28e17f72
//��32λ����ת��Ϊ16�����ַ����������
string Int_HexStr(int input)
{
	const char alpht[] = "0123456789abcdef";

	//���ɵ����յ�16�����ַ���
	string hex_String = "";

	for (int i = 0; i < 4; i++)
	{
		//ÿ��ȡinput�ĵ�8λ����Ϊ�������Ĵ�������С�˷�ʽ�洢�ģ����Դӵ�λ��ʼȡ��ת�����ַ�������ǰͷ��
		unsigned hex_Num = (input >> (i * 8)) & 0xff;

		//���Ͱ�λת���������ַ���ƴ������ʱ�ַ�����
		string temp = "";
		temp += alpht[hex_Num / 16];
		temp += alpht[hex_Num % 16];

		//���ڽ�����
		hex_String += temp;
	}
	return hex_String;
}

//MD5ѹ������
string MD5(string m)
{
	//��������ʼֵ
	unsigned int A = 0x67452301;
	unsigned int B = 0xefcdab89;
	unsigned int C = 0x98badcfe;
	unsigned int D = 0x10325476;

	//��ѹ����Ϣ���ֽڳ���
	int len_byte = m.length();
	//������ĳ��Ƚ��з��飬����Ϊgroup_num��(��䣺������α������512bit������ֱ��group_num��һ���ɣ�����Ҫ��64λ�洢��Ϣ����)
	int group_num = ((len_byte + 8) / 64) + 1;
	//32λ�޷�����������洢����Ϣ�����32λ������ʽ
	unsigned int* m_byte = new unsigned int[group_num * 16];
	memset(m_byte, 0, sizeof(unsigned int) * group_num * 16);
	//������Ϣ
	for (int i = 0; i < len_byte; i++)
	{
		//��Ϣ��ÿ�ĸ��ַ�������32λ���洢Ϊm_byte��һ��Ԫ��
		m_byte[i / 4] |= m[i] << ((i % 4) * 8);
	}
	//���λ
	m_byte[len_byte >> 2] |= 0x80 << ((len_byte % 4) * 8);
	//���64bit����Ϣ����
	m_byte[group_num * 16 - 2] = len_byte * 8;

	//���������ĸ��Ĵ���
	unsigned int a, b, c, d;
	for (int i = 0; i < group_num; i++)
	{
		//����һ�ε��м���Ϊ��������ֵ
		a = A;
		b = B;
		c = C;
		d = D;
		//ÿһ���Ļ����߼������Ľ��
		unsigned int g;
		//����X[k]Ѱַ�������ĵ�ĳһ�����ĳһ���ֵ�Ѱַ
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

//��������16�����ַ���������Ķ����ƴ��Ĳ�ͬ��λ�������ڼ���ѩ��ЧӦ
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
		cout << "��ѡ����Ĳ���:[0:�����ַ�����MD5; 1:�����ļ���Ϣ��MD5; 2:����ѩ��ЧӦ; q:�˳�����]" << endl;
		char flag;
		cin >> flag;
		if (flag == '0')
		{
			cout << "����������: ";
			string m = ""; cin >> m;
			cout << "���ĵ�MD5ֵ: " << MD5(m) << endl;
		}
		else if (flag == '1')
		{
			cout << "�������ļ�·��: ";
			string filepath = ""; cin >> filepath;
			ifstream file(filepath.data());
			ostringstream buf;
			char c;
			while (buf && file.get(c))
				buf.put(c);
			string input = buf.str();
			cout << "��Ϣ: " << input << endl << "��Ϣ��MD5ֵ: " << MD5(input) << endl;
		}
		else if (flag == '2')
		{
			cout << "���Ե�ԭʼ��Ϣ: ";
			//string m = ""; cin >> m;
			string m = "abcdefgh"; cout << m << endl;
			string origin = MD5(m);
			cout << "ԭʼ��Ϣ��MD5ֵ: " << origin << endl;
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

			cout << "��1�β���" << endl << "����Ϣ�ı�Ϊ: " << change1 << endl;
			cout << "�ı�1��MD5ֵ: " << MD5(change1) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[0] << endl;
			cout << endl;

			cout << "��2�β���" << endl << "����Ϣ�ı�Ϊ: " << change2 << endl;
			cout << "�ı�2��MD5ֵ: " << MD5(change2) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[1] << endl;
			cout << endl;

			cout << "��3�β���" << endl << "����Ϣ�ı�Ϊ: " << change3 << endl;
			cout << "�ı�3��MD5ֵ: " << MD5(change3) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[2] << endl;
			cout << endl;

			cout << "��4�β���" << endl << "����Ϣ�ı�Ϊ: " << change4 << endl;
			cout << "�ı�4��MD5ֵ: " << MD5(change4) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[3] << endl;
			cout << endl;

			cout << "��5�β���" << endl << "����Ϣ�ı�Ϊ: " << change5 << endl;
			cout << "�ı�5��MD5ֵ: " << MD5(change5) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[4] << endl;
			cout << endl;

			cout << "��6�β���" << endl << "����Ϣ�ı�Ϊ: " << change6 << endl;
			cout << "�ı�6��MD5ֵ: " << MD5(change6) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[5] << endl;
			cout << endl;

			cout << "��7�β���" << endl << "����Ϣ�ı�Ϊ: " << change7 << endl;
			cout << "�ı�7��MD5ֵ: " << MD5(change7) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[6] << endl;
			cout << endl;

			cout << "��8�β���" << endl << "����Ϣ�ı�Ϊ: " << change8 << endl;
			cout << "�ı�8��MD5ֵ: " << MD5(change8) << endl;
			cout << "MD5ֵ�ı��λ��: " << change[7] << endl;
			cout << endl;

			cout << "ƽ���ı�λ��: " << avg << endl;
		}
		else if (flag == 'q')
		{
			cout << "�ټ���" << endl;
			break;
		}
		else
		{
			cout << "��������ȷָ�" << endl;
			continue;
		}
	}

	system("pause");
	return 0;
}