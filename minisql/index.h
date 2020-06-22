#ifndef _INDEX_H
#define _INDEX_H

// Ŀǰ����ֻ���г��� api ģ����Ҫ�ĺ�����

#include <string>
#include "minisql.h"
using namespace std;

// initIndex ����������һ���������Ƶ�������
// ���� SQL ��ͬ���ݿ������������������������������ĸ�ʽΪ"<���ݿ���>-<������>"�������Դ���Ϊ���������֡�
// ���������������������('d'Ϊ����, 'c'Ϊ�ַ�, 'f'Ϊ������)�����ȡ�
void initIndex(const string, const char, const int);
// deleteIndex ������ɾ���������Ƶ�������
void deleteIndex(const string);
// insertIndexNode ��������������Ƶ������в���ֵ��
// �����������������ֵ���� record �е�ƫ������[�����ַ����ĳ���]��
void insertIndexNode(const string, const int, const int);
void insertIndexNode(const string, const float, const int);
void insertIndexNode(const string, const FixedString, const int, const int);
// deleteIndexNode �������ڸ������Ƶ�������ɾ��ĳ��ֵ��
// �����������������ֵ��
void deleteIndexNode(const string, const int);
void deleteIndexNode(const string, const float);
void deleteIndexNode(const string, const FixedString);
// findIndexNode ������Ѱ��������ĳ��ֵ�� record �е�ƫ������-1�����Ҳ�����
// �����������������ֵ��
int findIndexNode(const string, const int);
int findIndexNode(const string, const float);
int findIndexNode(const string, const FixedString);
// findIndexNodes ������Ѱ�������д���(����)ĳ��ֵ�Ľڵ��� record �е�ƫ������-1�����Ҳ�����
// �����������������ֵ���Ƿ�������ڸ�ֵ�Ľڵ�(true Ϊ����)
// �����ƫ�������С�������У�
vector<int> findIndexNodes(const string, const int, const bool);
vector<int> findIndexNodes(const string, const float, const bool);
// findIndexNodes ������Ѱ����������ĳ��ֵ����Ľڵ��� record �е�ƫ������-1�����Ҳ�����
// ������������������������ޣ��Ƿ�����������ޣ��������ޣ��Ƿ�����������ޡ�
// �����ƫ�������С�������У�
vector<int> findIndexNodes(const string, const float, const bool, const float, const bool);
// clearIndex ��������ո������Ƶ������е�ȫ��ֵ(ע������ղ���ɾ����)��
void clearIndex(const string);
#endif