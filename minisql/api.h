/********************
 * api.h: API模块，负责将解释器所生成的结果传给各处理模块进行处理。
 */

#ifndef _API_H
#define _API_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// api 模块在创建数据表时传入的字段的格式。
// fieldName 为字段的名称。
// fieldType 为字段的类型，可能为"int", "char"和"float"。
// fieldSize 为该字段的大小，仅针对类型为 char 的情形，其余情况为0。
// unique 为值是否唯一。
struct field {
	string fieldName;
	string fieldType;
	int fieldSize;
	bool unique;
};

class Condition
{
public:
	char type;		// 条件中数据的类型。'd'为整形, 'c'为字符, 'f'为浮点数。
	string attr;	// 该条件涉及的属性名。
	string op;		// 条件的操作符。一共有如下情况："EQ", "DIFF", "LT", "GT", "LTE", "GTE"。
	struct {
		int d;
		FixedString c;
		float f;
	} value;		// 条件涉及的属性值，其中仅 op 为"EQ"和"DIFF"时可以为 FixedString。

	Condition(const string attr, const string op, const int value)
	{
		this->type = 'd';
		this->attr = attr;
		this->op = op;
		this->value.d = value;
	}

	Condition(const string attr, const string op, const float value)
	{
		this->type = 'f';
		this->attr = attr;
		this->op = op;
		this->value.f = value;
	}

	Condition(const string attr, const string op, const FixedString value)
	{
		this->type = 'c';
		this->attr = attr;
		this->op = op;
		this->value.c = value;
	}
/*
	Condition(const string attr, const int value1, const int value2)
	{
		this->type = 'd';
		this->attr = attr;
		this->op = "BETW";
		this->value1.d = value1;
		this->value2.d = value2;
	}

	Condition(const string attr, const float value1, const float value2)
	{
		this->type = 'f';
		this->attr = attr;
		this->op = "BETW";
		this->value1.f = value1;
		this->value2.f = value2;
	}*/
};

// 条件不符合，不用继续匹配了。
class DoNotMatch{};

// executeCommand函数：给定指定格式的经 interpreter 初步处理过的指令，进行具体的处理。
void executeCommand(const struct command);

#endif
