#ifndef _MINISQL_H
#define _MINISQL_H

// ����һЩ������Ŀ����Ҫ�õ������ݡ�

# include <string>
using namespace std;

// sql ���ִ�й����еĴ���
class SqlError : exception
{
	string message;
public:
	SqlError(string message)
	{
		this->message = message;
	}
	void printError()
	{
		cout << "error: " << message << endl;
	}
};

// �������ַ�����ĩβ���貹0.
// Ŀǰֻ��д��һ���ֹ��ܣ�����ģ���Ƚϵȹ��ܣ�����Ҫ�õ�ͬѧ����ȫ��
class FixedString
{
	char *content;
	int size;
public:
	FixedString()
	{
		size = 0;
		content = NULL;
	}

	FixedString(int size, char *str)
	{
		// ����֤ str �ĳ��ȷ���Ҫ����Ҫ�����жϡ������ȳ��������н�ȡ��
		this->size = size;
		content = new char[size];
		bool flag = false;
		for (int i = 0; i < size; i++)
		{
			if (!flag)
			{
				if (str[i] != '\0')
				{
					content[i] = str[i];
				}
				else
				{
					flag = true;
				}
			}
			if (flag)
			{
				content[i] = '\0';	// ����Ĳ�����0��ȫ��
			}
		}
	}

	FixedString(int size, string str)
	{
		// ����֤ str �ĳ��ȷ���Ҫ����Ҫ�����жϡ������ȳ��������н�ȡ��
		this->size = size;
		content = new char[size];
		for (int i = 0; i < size; i++)
		{
			if (i < str.length())
			{
				content[i] = str[i];
			}
			else
			{
				content[i] = '\0';
			}
		}
	}

	FixedString(const FixedString &str)
	{
		this->size = str.size;
		this->content = new char[size];
		for (int i = 0; i < size; i++)
		{
			this->content[i] = str.content[i];
		}
	}

	~FixedString()
	{
		delete content;
	}

	int size()
	{
		return size;
	}

	char operator [] (int i)
	{
		return content[i];
	}

	operator char* ()
	{
		return content;
	}

	operator string ()
	{
		return static_cast<string>(content);
	}

	// TODO: ������ char* / string / FixedString ���бȽϵ���������أ�����
};

#endif