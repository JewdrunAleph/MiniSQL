/********************
 * api.h: APIģ�飬���𽫽����������ɵĽ������������ģ����д���
 */
#ifndef _API_H
#define _API_H

#include <iostream>
#include <string>
#include <vector>
#include "minisql.h"
#include "record.h"
using namespace std;

// api ģ���ڴ������ݱ�ʱ������ֶεĸ�ʽ��
// fieldName Ϊ�ֶε����ơ�
// fieldType Ϊ�ֶε����ͣ�����Ϊ"int", "char"��"float"��
// fieldSize Ϊ���ֶεĴ�С�����������Ϊ char �����Σ��������Ϊ0��
// unique Ϊֵ�Ƿ�Ψһ��
struct field {
	string fieldName;
	string fieldType;
	unsigned int fieldSize;
	bool unique;
};

// �����ṩ�� record manager �ļ�¼��Χ��
struct recordRange {
	vector<int> range;		// ��¼�����Χ��
	vector<int> excluded;	// ��Ҫ�ų��ļ�¼��
	bool norange;			// �Ƿ�Ŀǰû�м�¼��Χ����rangeΪ�ս������֣�
};

// Ϊ����������жϵ�����Ч�ʣ�����������ö�����͡�
enum condop {UNDEFINED=0, EQ=1, DIFF=2, LT=3, GT=4, LTE=5, GTE=6, BETW=7};

class Condition
{
public:
	char type = '\0';		// ���������ݵ����͡�'d'Ϊ����, 'c'Ϊ�ַ�, 'f'Ϊ��������
	string attr = "";		// �������漰����������
	condop op = UNDEFINED;	// �����Ĳ�������һ�������������"EQ", "DIFF", "LT", "GT", "LTE", "GTE"��"BETW"��
	struct {
		int d = 0;
		FixedString c = FixedString(0, "");
		float f = 0.0;
	} value1;				// �����漰������ֵ�����н� op Ϊ"EQ"��"DIFF"ʱ����Ϊ FixedString��
	struct {
		int d = 0;
		float f = 0.0;
	} value2;				// �����漰������ֵ���� op Ϊ"BETW"ʱʹ�ã�Ϊ����ֵ�еĽϴ��ߡ�
	bool eq1 = false;		// ����Ϊ"BETW"ʱ���½��ܷ�ȡ����
	bool eq2 = false;		// ����Ϊ"BETW"ʱ���Ͻ��ܷ�ȡ����

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

	// ��ȡֵ���ڱȽ�
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

	// �ı�ֵ��
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

// ���������ϣ����ü���ƥ���ˡ�
class DoNotMatch{};

// executeCommand����������ָ����ʽ�ľ� interpreter �����������ָ����о���Ĵ���
void executeCommand(const struct command);

// ��ģ���ṩ��һ�������ж������ĺ�����

// inCondInterval �������ж�һ����ֵ�Ƿ��������������ڡ�
bool inCondInterval(float value, const Condition cond);


// ģ���ڲ�ʹ�õĺ���������ģ���벻Ҫʹ�á�

void tableCreateProcess(const string, vector<struct field>&, string &);
void recordInsertProcess(const string, const string, const vector<struct field>, vector<Record_node> &);
void conditionProcess(const string, const vector<struct field>, vector<Condition> &);
void fieldsProcess(const string, const string, vector<string> &);
struct recordRange recordProcess(const string, vector<Condition> &);
vector<int> mergeOffsets(vector<int>, vector<int>);
vector<int> unionOffsets(vector<int>, vector<int>);
vector<int> excludeOffsets(vector<int>, vector<int>);
#endif
