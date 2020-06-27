#ifndef _MINISQL_H
#define _MINISQL_H

// 定义一些整个项目都需要用到的内容。
#include <fstream>
#include <string>
#include"Interpreter.h"
#include"api.h"
using namespace std;

// sql 语句执行过程中的错误。
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

// 因为代码调用问题导致的错误。
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

// 定长的字符串，末尾无需补0.
// 目前只编写了一部分功能，其余的（如比较等功能）请需要用的同学来补全。
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
		// 不保证 str 的长度符合要求，需要自行判断。若长度超出则会进行截取。
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
				content[i] = '\0';	// 多余的部分用0补全。
			}
		}
	}

	FixedString(unsigned int size, string str)
	{
		// 不保证 str 的长度符合要求，需要自行判断。若长度超出则会进行截取。
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

	// TODO: 增加与 char* / string / FixedString 进行比较的运算符重载！！！
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
