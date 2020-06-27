/********************
 * api.h: API模块，负责将解释器所生成的结果传给各处理模块进行处理。
 */
#ifndef _API_H
#define _API_H

#include <iostream>
#include <string>
#include <vector>
#include "minisql.h"
#include "record.h"
using namespace std;

// api 模块在创建数据表时传入的字段的格式。
// fieldName 为字段的名称。
// fieldType 为字段的类型，可能为"int", "char"和"float"。
// fieldSize 为该字段的大小，仅针对类型为 char 的情形，其余情况为0。
// unique 为值是否唯一。
struct field {
	string fieldName;
	string fieldType;
	unsigned int fieldSize;
	bool unique;
};

// 用于提供给 record manager 的记录范围。
struct recordRange {
	vector<int> range;		// 记录的最大范围。
	vector<int> excluded;	// 需要排除的记录。
	bool norange;			// 是否目前没有记录范围（和range为空进行区分）
};

// 为了提高条件判断的运行效率，条件采用了枚举类型。
enum condop {UNDEFINED=0, EQ=1, DIFF=2, LT=3, GT=4, LTE=5, GTE=6, BETW=7};

class Condition
{
public:
	char type = '\0';		// 条件中数据的类型。'd'为整形, 'c'为字符, 'f'为浮点数。
	string attr = "";		// 该条件涉及的属性名。
	condop op = UNDEFINED;	// 条件的操作符。一共有如下情况："EQ", "DIFF", "LT", "GT", "LTE", "GTE"，"BETW"。
	struct {
		int d = 0;
		FixedString c = FixedString(0, "");
		float f = 0.0;
	} value1;				// 条件涉及的属性值，其中仅 op 为"EQ"和"DIFF"时可以为 FixedString。
	struct {
		int d = 0;
		float f = 0.0;
	} value2;				// 条件涉及的属性值，仅 op 为"BETW"时使用，为两个值中的较大者。
	bool eq1 = false;		// 条件为"BETW"时，下界能否取到。
	bool eq2 = false;		// 条件为"BETW"时，上界能否取到。

	Condition() {}

	Condition(const string attr, const condop op, const int value)
	{
		this->type = 'd';
		this->attr = attr;
		this->op = op;
		this->value1.d = value;
	}

	Condition(const string attr, const condop op, const float value)
	{
		this->type = 'f';
		this->attr = attr;
		this->op = op;
		this->value1.f = value;
	}

	Condition(const string attr, const condop op, const FixedString value)
	{
		this->type = 'c';
		this->attr = attr;
		this->op = op;
		this->value1.c = value;
	}

	Condition(const string attr, const int value1, const bool eq1, const int value2, const bool eq2)
	{
		this->type = 'd';
		this->attr = attr;
		this->op = BETW;
		this->value1.d = value1;
		this->value2.d = value2;
		this->eq1 = eq1;
		this->eq2 = eq2;
	}

	Condition(const string attr, const float value1, const bool eq1, const float value2, const bool eq2)
	{
		this->type = 'f';
		this->attr = attr;
		this->op = BETW;
		this->value1.f = value1;
		this->value2.f = value2;
		this->eq1 = eq1;
		this->eq2 = eq2;
	}

	// 获取值用于比较
	const float getValue1() const
	{
		if (type == 'f')
		{
			return this->value1.f;
		}
		else if (type == 'd')
		{
			return static_cast<float>(this->value1.d);
		}
		else
		{
			throw CodeError("condition getvalue1: invalid condition type!");
		}
	}

	const float getValue2() const
	{
		if (type == 'f')
		{
			return this->value2.f;
		}
		else if (type == 'd')
		{
			return static_cast<float>(this->value2.d);
		}
		else
		{
			throw CodeError("condition getvalue1: invalid condition type!");
		}
	}

	// 改变值。
	void setValue1(const float value)
	{
		if (type == 'f')
		{
			this->value1.f = value;
		}
		else if (type == 'd')
		{
			this->value1.d = static_cast<int>(value);
		}
	}

	void setValue2(const float value)
	{
		if (type == 'f')
		{
			this->value2.f = value;
		}
		else if (type == 'd')
		{
			this->value2.d = static_cast<int>(value);
		}
	}
};

// 条件不符合，不用继续匹配了。
class DoNotMatch{};

// executeCommand函数：给定指定格式的经 interpreter 初步处理过的指令，进行具体的处理。
void executeCommand(const struct command);

// 本模块提供了一个用于判断条件的函数。

// inCondInterval 函数：判断一个数值是否在条件的区间内。
bool inCondInterval(float value, const Condition cond);


// 模块内部使用的函数，其他模块请不要使用。

void tableCreateProcess(const string, vector<struct field>&, string &);
void recordInsertProcess(const string, const string, const vector<struct field>, vector<Record_node> &);
void conditionProcess(const string, const vector<struct field>, vector<Condition> &);
void fieldsProcess(const string, const string, vector<string> &);
struct recordRange recordProcess(const string, vector<Condition> &);
vector<int> mergeOffsets(vector<int>, vector<int>);
vector<int> unionOffsets(vector<int>, vector<int>);
vector<int> excludeOffsets(vector<int>, vector<int>);
#endif
