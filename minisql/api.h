/********************
 * api.h: APIģ�飬���𽫽����������ɵĽ������������ģ����д���
 */

#ifndef _API_H
#define _API_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// api ģ���ڴ������ݱ�ʱ������ֶεĸ�ʽ��
// fieldName Ϊ�ֶε����ơ�
// fieldType Ϊ�ֶε����ͣ�����Ϊ"int", "char"��"float"��
// fieldSize Ϊ���ֶεĴ�С�����������Ϊ char �����Σ��������Ϊ0��
// unique Ϊֵ�Ƿ�Ψһ��
struct field {
	string fieldName;
	string fieldType;
	int fieldSize;
	bool unique;
};

class Condition
{
public:
	char type;		// ���������ݵ����͡�'d'Ϊ����, 'c'Ϊ�ַ�, 'f'Ϊ��������
	string attr;	// �������漰����������
	string op;		// �����Ĳ�������һ�������������"EQ", "DIFF", "LT", "GT", "LTE", "GTE"��
	struct {
		int d;
		FixedString c;
		float f;
	} value;		// �����漰������ֵ�����н� op Ϊ"EQ"��"DIFF"ʱ����Ϊ FixedString��

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

// ���������ϣ����ü���ƥ���ˡ�
class DoNotMatch{};

// executeCommand����������ָ����ʽ�ľ� interpreter �����������ָ����о���Ĵ���
void executeCommand(const struct command);

#endif
