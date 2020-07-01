#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

/* 致 interpreter 模块：
 * api 模块中对数据表、数据库、索引名的要求如下(基本同 mySQL 的要求)：
 * 1. 名称只能包括数字、大小写字母和下划线；
 * 2. 没有首字母不能为数字的要求。
 * 希望在语法检查时注意。
 */

#include<iostream>
//#include<stdlib.h>
//#include<math.h>
#include <algorithm>
#include <string>
#include "Minisql.h"
using namespace std;

// api 模块所需要的命令的格式。
// op 为2位数字，代表命令的类型。
// arg1..为命令的内容。具体根据不同命令而定，见 executeCommand 函数。
// 注意：此时需要保证各个 arg 的内容不允许有换行。换行应该是 intrepreter 已经处理掉的。
struct command {
	char op[3];  // 命令的类型。
	string arg1; // 命令的第一个参数。
	string arg2; // 命令的第二个参数。
	string arg3; // 命令的第三个参数。
};

//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
command Interpreter(string statement);
//读取用户输入
string read_input();
//验证create语句是否有效
command create_clause(string SQL, int start,command cmd);
//验证create database语句是否有效
command create_database(string SQL, int start,command cmd);
//验证create table语句是否有效
command create_table(string SQL, int start,command cmd);
////获得属性
//command get_attribute(string temp, string sql,command cmd);
//验证create index语句是否有效
command create_index(string SQL, int start,command cmd);
//验证create index on语句是否有效
command create_index_on(string SQL, int start, string sql,command cmd);
//验证drop语句是否有效
command drop_clause(string SQL, int start,command cmd);
//验证drop database语句是否有效
command drop_database(string SQL, int start,command cmd);
//验证drop table语句是否有效
command drop_table(string SQl, int start,command cmd);
//验证drop index语句是否有效
command drop_index(string SQL, int start,command cmd);
//验证select 语句是否有效
command select_clause(string SQL, int start,command cmd);
////获得属性组或文件组的每一项
//string get_part(string temp, string sql, string kind);
//验证insert 语句是否有效
command insert_clause(string SQL, int start,command cmd);
//验证insert into values语句是否有效
command insert_into_values(string SQL, int start, string sql,command cmd);
//验证delete语句是否有效
command delete_clause(string SQL, int start,command cmd);
//验证 delete from where 语句是否有效
command delete_from_where(string SQL, int start, string sql,command cmd);
////将表达式转化为内部形式
//string get_expression(string temp, string sql);
////获取表达式组的每个表达式
//string get_each(string T, string sql, string condition);
//验证use语句是否有效
command use_clause(string SQL, int start,command cmd);
//验证execfile语句是否有效
command execfile_clause(string SQL, int start,command cmd);
//验证quit语句是否有效
command quit_clause(string SQL, int start,command cmd);

#endif
