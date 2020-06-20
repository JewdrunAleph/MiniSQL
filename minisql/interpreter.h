#ifndef _INTERPRETER_H
#define _INTERPRETER_H


/* 致 interpreter 模块：
 * api 模块中对数据表、数据库、索引名的要求如下(基本同 mySQL 的要求)：
 * 1. 名称只能包括数字、大小写字母和下划线；
 * 2. 没有首字母不能为数字的要求。
 * 希望在语法检查时注意。
 */

// 目前这里只是列出了 api 模块需要的数据的格式。

#include <string>
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

#endif