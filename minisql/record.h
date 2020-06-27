#ifndef _RECORD_H
#define _RECORD_H

// Ŀǰ����ֻ���г��� api ģ����Ҫ�ĺ�����

#include <string>
#include <vector>
#include "api.h"
#include "minisql.h"
using namespace std;

// ĳһ�ֶ�ĳһ��¼��ֵ����Ϣ��
class Value_node
{
public:
	char type;   // �����ݵ����͡�'d'Ϊ����, 'c'Ϊ�ַ�, 'f'Ϊ��������
	int size;
	struct {
		int d;
		FixedString c;
		float f;
	} data;
	int offset;  // �����ݴ�ŵ�λ�á�

	Value_node(const int data, const int offset)
	{
		this->type = 'd';
		this->size = sizeof(int);
		this->data.d = data;
		this->offset = offset;
	}

	Value_node(const float data, const int offset)
	{
		this->type = 'f';
		this->size = sizeof(float);
		this->data.f = data;
		this->offset = offset;
	}

	Value_node(const char *const data, const int size, const int offset)
	{
		this->type = 'c';
		this->size = size;
		this->data.c = FixedString(size, data);
		this->offset = offset;
	}
};

// ĳһ�ֶεļ�¼��Ϣ��
class Record_node
{
public:
	char type;		// �����ݵ����͡�'d'Ϊ����, 'c'Ϊ�ַ�, 'f'Ϊ��������
	int size;
	string attr;	// ��¼����������
	struct {
		int d;
		FixedString c;
		float f;
	} data;			// ��¼������ֵ��

	Record_node(const string attr, const int data)
	{
		this->type = 'd';
		this->attr = attr;
		this->data.d = data;
	}

	Record_node(const string attr, const float data)
	{
		this->type = 'f';
		this->attr = attr;
		this->data.f = data;
	}

	Record_node(const string attr, const int size, const char *data)
	{
		this->type = 'c';
		this->attr = attr;
		this->data.c = FixedString(size, data);
	}

	Record_node(const string attr, FixedString data)
	{
		this->type = 'c';
		this->attr = attr;
		this->data.c = data;
	}
};

struct Record
{
	int offset;
	vector<Record_node> list;
};

class Condition;

// initTable ���������ݸ������ֶ��б�(��ʽ�� api.h �Ķ���)����һ�����ݱ�
// ��������ֱ�Ϊ�����ݱ������ֶ��б�
// ���ڲ�ͬ���ݿ������ݱ����������������ݱ�������ĸ�ʽΪ"<���ݿ���>-<���ݱ���>"�����Դ���Ϊ���ݱ�����ơ�����ͬ��
void initTable(const string, const vector<struct field>);
// deleteTable ������ɾ��һ���������Ƶ����ݱ����� miniSQL ��Ҫ���б�������Լ�������ֱ��ɾ�����ɡ�
void deleteTable(const string);
// valuesInField ��������ȡһ�� unique �ֶ��µ�����ֵ������λ�ã��� index manager ʹ�á�
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")���ֶ�����
const vector<Value_node> valuesInField(const string, const string);
// getUniqueOffset ��������ȡһ�� unique �ֶ��µĸ���ֵ��λ�á����޸�ֵ�򷵻� -1��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")���ֶ�����������ֵ��
const int getUniqueOffset(const string, const string, const int);
const int getUniqueOffset(const string, const string, const float);
const int getUniqueOffset(const string, const string, const FixedString);
// insertRecord ����������ڲ���һ����¼��unique �� char �ĳ��Ⱦ����Եõ���֤�����ز���ļ�¼�� offset��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")����¼��Ϣ��
const int insertRecord(const string, const vector<Record_node>);
// clearRecord ��������ո������Ƶ����ݱ��ڵ����м�¼��(ע������ղ���ɾ����)
void clearRecord(const string);
// findRecords ���������ݸ�����������ĳ���ݱ������м�¼�з��������ļ�¼��������ֵ��
// ÿһ����¼�ֱ�����һ�� vector<Record_node>������ offset ��� struct Record �����ҽ���Щ��¼������һ�� vector��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")���������ų���Χ(�����ų���Χ�ڵļ�¼�����ж�ֱ���ų�)��
const vector<struct Record> findRecords(const string, const vector<Condition>, const vector<int>);
// findRecords ���������ݸ�����������ĳ���ݱ��ڵ�ĳһ��Χ�ļ�¼�з��������ļ�¼��������ֵ��
// ÿһ����¼�ֱ�����һ�� vector<Record_node>������ offset ��� struct Record �����ҽ���Щ��¼������һ�� vector��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")�����������ҷ�Χ(��ֻ�ڶ�Ӧ offset �ļ�¼�н�����һ�ж�)���ų���Χ��
const vector<struct Record> findRecords(const string, const vector<Condition>, const vector<int>, const vector<int>);
// deleteRecord ������ɾ��ָ�����ݱ��һ��ָ�� offset �ļ�¼��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")��ƫ������
void deleteRecord(const string, const int);
// printRecords ���������ݸ�����������ĳ���ݱ������м�¼������Ļ���������Ϣ��
// ÿһ����¼�ֱ�����һ�� vector<Record_node>������ offset ��� struct Record �����ҽ���Щ��¼������һ�� vector��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")��������ֶ��б�
const vector<struct Record> printRecords(const string, const vector<string>);
// printRecords ���������ݸ�����������ĳ���ݱ������м�¼�з��������ļ�¼������Ļ���������Ϣ��
// ÿһ����¼�ֱ�����һ�� vector<Record_node>������ offset ��� struct Record �����ҽ���Щ��¼������һ�� vector��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")��������������ֶ��б��ų���Χ(�����ų���Χ�ڵļ�¼�����ж�ֱ���ų�)��
const vector<struct Record> printRecords(const string, const vector<Condition>, const vector<string>, const vector<int>);
// printRecords ���������ݸ�����������ĳ���ݱ��ڵ�ĳһ��Χ�ļ�¼�з��������ļ�¼������Ļ���������Ϣ��
// ÿһ����¼�ֱ�����һ�� vector<Record_node>������ offset ��� struct Record �����ҽ���Щ��¼������һ�� vector��
// ��������ֱ�Ϊ�����ݱ���(��ʽΪ"<���ݿ���>-<���ݱ���>")��������������ֶ��б����ҷ�Χ(��ֻ�ڶ�Ӧ offset �ļ�¼�н�����һ�ж�)���ų���Χ��
const vector<struct Record> printRecords(const string, const vector<Condition>, const vector<string>, const vector<int>, const vector<int>);
// printNoRecordFound ����������Ļ�����ѯ���Ϊ�ա�
void printNoRecordFound();
#endif