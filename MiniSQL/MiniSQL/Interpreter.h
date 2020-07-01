#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

/* �� interpreter ģ�飺
 * api ģ���ж����ݱ����ݿ⡢��������Ҫ������(����ͬ mySQL ��Ҫ��)��
 * 1. ����ֻ�ܰ������֡���Сд��ĸ���»��ߣ�
 * 2. û������ĸ����Ϊ���ֵ�Ҫ��
 * ϣ�����﷨���ʱע�⡣
 */

#include<iostream>
//#include<stdlib.h>
//#include<math.h>
#include <algorithm>
#include <string>
#include "Minisql.h"
using namespace std;

// api ģ������Ҫ������ĸ�ʽ��
// op Ϊ2λ���֣�������������͡�
// arg1..Ϊ��������ݡ�������ݲ�ͬ����������� executeCommand ������
// ע�⣺��ʱ��Ҫ��֤���� arg �����ݲ������л��С�����Ӧ���� intrepreter �Ѿ�������ġ�
struct command {
	char op[3];  // ��������͡�
	string arg1; // ����ĵ�һ��������
	string arg2; // ����ĵڶ���������
	string arg3; // ����ĵ�����������
};

//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
command Interpreter(string statement);
//��ȡ�û�����
string read_input();
//��֤create����Ƿ���Ч
command create_clause(string SQL, int start,command cmd);
//��֤create database����Ƿ���Ч
command create_database(string SQL, int start,command cmd);
//��֤create table����Ƿ���Ч
command create_table(string SQL, int start,command cmd);
////�������
//command get_attribute(string temp, string sql,command cmd);
//��֤create index����Ƿ���Ч
command create_index(string SQL, int start,command cmd);
//��֤create index on����Ƿ���Ч
command create_index_on(string SQL, int start, string sql,command cmd);
//��֤drop����Ƿ���Ч
command drop_clause(string SQL, int start,command cmd);
//��֤drop database����Ƿ���Ч
command drop_database(string SQL, int start,command cmd);
//��֤drop table����Ƿ���Ч
command drop_table(string SQl, int start,command cmd);
//��֤drop index����Ƿ���Ч
command drop_index(string SQL, int start,command cmd);
//��֤select ����Ƿ���Ч
command select_clause(string SQL, int start,command cmd);
////�����������ļ����ÿһ��
//string get_part(string temp, string sql, string kind);
//��֤insert ����Ƿ���Ч
command insert_clause(string SQL, int start,command cmd);
//��֤insert into values����Ƿ���Ч
command insert_into_values(string SQL, int start, string sql,command cmd);
//��֤delete����Ƿ���Ч
command delete_clause(string SQL, int start,command cmd);
//��֤ delete from where ����Ƿ���Ч
command delete_from_where(string SQL, int start, string sql,command cmd);
////�����ʽת��Ϊ�ڲ���ʽ
//string get_expression(string temp, string sql);
////��ȡ���ʽ���ÿ�����ʽ
//string get_each(string T, string sql, string condition);
//��֤use����Ƿ���Ч
command use_clause(string SQL, int start,command cmd);
//��֤execfile����Ƿ���Ч
command execfile_clause(string SQL, int start,command cmd);
//��֤quit����Ƿ���Ч
command quit_clause(string SQL, int start,command cmd);

#endif
