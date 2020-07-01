#ifndef _CATALOG_H
#define _CATALOG_H

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <utility>

using namespace std;

// Ŀǰ����ֻ���г� api ģ������Ҫ�õ��� catalog ģ�����غ����� api ��Ҫ�����ݵĸ�ʽ��


// databaseExists �������ж�һ���������Ƶ����ݿ��Ƿ��Ѿ����ڡ��������򷵻� true���������򷵻� false��
const bool databaseExists(const string);
// createDatabase ����������һ���������Ƶ����ݿ⡣
void createDatabase(const string);
// dropDatabase ������ɾ��һ���������Ƶ����ݿ⡣
void dropDatabase(const string);
// tableExists �������ж�һ���������Ƶ����ݱ��Ƿ��Ѿ����ڡ��������򷵻� true���������򷵻� false��
// ��������ֱ�Ϊ�����ݿ��������ݱ�����
const bool tableExists(const string, const string);
// createTable ���������ݸ������ֶ��б�(��ʽ�� api.h �Ķ���)������(����������Ϊ"")����һ�����ݱ�
// ��������ֱ�Ϊ�����ݿ��������ݱ������ֶ��б��������ơ�
void createTable(const string, const string, const vector<struct field>, const string);
// dropTable ������ɾ��һ���������Ƶ����ݱ���ʧ����Ҫ���ԭ��
// ��������ֱ�Ϊ�����ݿ��������ݱ�����
void dropTable(const string, const string);
// getFields ��������ȡһ�����ݱ��е������ֶε���Ϣ��
// ��������ֱ�Ϊ�����ݿ��������ݱ�����
const vector<struct field> getFields(const string, const string);
// getField ��������ȡһ�����ݱ��е�ĳ���ֶε���Ϣ������ֶβ�������fieldName����Ϊ�ա�
// ��������ֱ�Ϊ�����ݿ��������ݱ������ֶ�����
const struct field getField(const string, const string, const string);
// isPrimaryKey �������ж�һ���ֶ��ǲ������롣
// ��������ֱ�Ϊ�����ݿ��������ݱ������ֶ�����
const bool isPrimaryKey(const string, const string, const string);
// getPrimaryKey ��������ȡһ�����ݱ��������ֶ������������뷵��""��
// ��������ֱ�Ϊ�����ݿ��������ݱ�����
const string getPrimaryKey(const string, const string);
// indexExists �������ж�һ���ֶ��Ƿ��Ѿ���������(ע����ģ���������Դ�����������������)��
// ��������ֱ�Ϊ�����ݿ��������ݱ������ֶ�����
// const bool indexExists(const string, const string, const string);
// indexExists �������ж�һ�����ݿ��Ƿ��Ѿ����ڸ������Ƶ�������
// ��������ֱ�Ϊ�����ݿ�������������
const bool indexExists(const string, const string);
// indexInField ��������ȡһ���ֶε�������������������""��
// ��������ֱ�Ϊ�����ݿ��������ݱ������ֶ�����
const string indexInField(const string, const string, const string);
// indiceInTable ��������ȡ�������Ƶ����ݱ��е����������������򷵻ؿա�
// ��������ֱ�Ϊ�����ݿ��������ݱ�����
const vector<string> indiceInTable(const string, const string);
// createIndex ����������һ���������Ƶ�������
// ��������ֱ�Ϊ�����ݿ���������������������������
void createIndex(const string D_str, const string T_str, const string A_str, const string I_str);
// dropIndex ������ɾ��һ���������Ƶ�������
// ��������ֱ�Ϊ�����ݿ�������������������
void dropIndex(const string D_str,  const string I_str);
// tablesInDatabase ��������ȡ�������Ƶ����ݿ���������ݱ�
const vector<string> tablesInDatabase(const string);
#endif

