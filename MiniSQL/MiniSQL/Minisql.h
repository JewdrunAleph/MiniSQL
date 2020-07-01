#ifndef _MINISQL_H
#define _MINISQL_H

// ����һЩ������Ŀ����Ҫ�õ������ݡ�
#include <fstream>
#include <string>
#include <iostream>

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

// ��Ϊ����������⵼�µĴ���
class CodeError : exception
{
	string message;
public:
	CodeError(string message)
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
	int stringSize;
public:
	FixedString()
	{
		stringSize = 0;
		content = NULL;
	}

	FixedString(int size, char *str)
	{
		// ����֤ str �ĳ��ȷ���Ҫ����Ҫ�����жϡ������ȳ��������н�ȡ��
		this->stringSize = size;
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

	FixedString(unsigned int size, string str)
	{
		// ����֤ str �ĳ��ȷ���Ҫ����Ҫ�����жϡ������ȳ��������н�ȡ��
		this->stringSize = size;
		content = new char[size];
		for (unsigned int i = 0; i < size; i++)
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
		this->stringSize = str.size();
		this->content = new char[stringSize];
		for (int i = 0; i < stringSize; i++)
		{
			this->content[i] = str.content[i];
		}
	}

	~FixedString()
	{
		delete content;
	}

	int size() const
	{
		return stringSize;
	}

	char operator [] (int i) const
	{
		return content[i];
	}

	operator char* () const
	{
		return content;
	}

	operator string () const
	{
		return static_cast<string>(content);
	}

	// TODO: ������ char* / string / FixedString ���бȽϵ���������أ�����
};


struct blockNode
{
	int offsetNum; // the offset number in the block list
	bool pin;  // the flag that this block is locked
	bool ifbottom; // flag that this is the end of the file node
	char* fileName; // the file which the block node belongs to
	friend class BufferManager;

private:
	char *address; // the content address
	blockNode * preBlock;
	blockNode * nextBlock;
	bool reference; // the LRU replacement flag
	bool dirty; // the flag that this block is dirty, which needs to written back to the disk later
	size_t using_size; // the byte size that the block have used. The total size of the block is BLOCK_SIZE . This value is stored in the block head.

};

struct fileNode
{
	char *fileName;
	bool pin; // the flag that this file is locked
	blockNode *blockHead;
	fileNode * nextFile;
	fileNode * preFile;
};

#endif
