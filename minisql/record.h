#ifndef _RECORD_H
#define _RECORD_H

// 目前这里只是列出了 api 模块需要的函数。

#include <string>
#include <vector>
#include "api.h"
#include "minisql.h"
using namespace std;

// 某一字段某一记录的值的信息。
class Value_node
{
public:
	char type;   // 本数据的类型。'd'为整形, 'c'为字符, 'f'为浮点数。
	int size;
	struct {
		int d;
		FixedString c;
		float f;
	} data;
	int offset;  // 本数据存放的位置。

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

// 某一字段的记录信息。
class Record_node
{
public:
	char type;		// 本数据的类型。'd'为整形, 'c'为字符, 'f'为浮点数。
	int size;
	string attr;	// 记录的属性名。
	struct {
		int d;
		FixedString c;
		float f;
	} data;			// 纪录的属性值。

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

// initTable 函数：根据给定的字段列表(格式见 api.h 的定义)创建一个数据表。
// 输入参数分别为：数据表名，字段列表。
// 由于不同数据库中数据表可能重名，因此数据表名输入的格式为"<数据库名>-<数据表名>"，可以此作为数据表的名称。后文同理。
void initTable(const string, const vector<struct field>);
// deleteTable 函数：删除一个给定名称的数据表。由于 miniSQL 的要求中表间无外键约束，因此直接删除即可。
void deleteTable(const string);
// valuesInField 函数：获取一个 unique 字段下的所有值及其存放位置，供 index manager 使用。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，字段名。
const vector<Value_node> valuesInField(const string, const string);
// getUniqueOffset 函数：获取一个 unique 字段下的给定值的位置。若无该值则返回 -1。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，字段名，给定的值。
const int getUniqueOffset(const string, const string, const int);
const int getUniqueOffset(const string, const string, const float);
const int getUniqueOffset(const string, const string, const FixedString);
// insertRecord 函数：向表内插入一个记录。unique 和 char 的长度均可以得到保证。返回插入的记录的 offset。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，记录信息。
const int insertRecord(const string, const vector<Record_node>);
// clearRecord 函数：清空给定名称的数据表内的所有记录。(注意是清空不是删除！)
void clearRecord(const string);
// findRecords 函数：根据给定条件查找某数据表内所有记录中符合条件的记录并返回其值。
// 每一条记录分别整成一个 vector<Record_node>，加上 offset 变成 struct Record ，并且将这些记录再整成一个 vector。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，条件，排除范围(即在排除范围内的记录不用判断直接排除)。
const vector<struct Record> findRecords(const string, const vector<Condition>, const vector<int>);
// findRecords 函数：根据给定条件查找某数据表内的某一范围的记录中符合条件的记录并返回其值。
// 每一条记录分别整成一个 vector<Record_node>，加上 offset 变成 struct Record ，并且将这些记录再整成一个 vector。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，条件，查找范围(即只在对应 offset 的记录中进行逐一判断)，排除范围。
const vector<struct Record> findRecords(const string, const vector<Condition>, const vector<int>, const vector<int>);
// deleteRecord 函数：删除指定数据表的一个指定 offset 的记录。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，偏移量。
void deleteRecord(const string, const int);
// printRecords 函数：根据给定条件查找某数据表内所有记录并向屏幕输出数据信息。
// 每一条记录分别整成一个 vector<Record_node>，加上 offset 变成 struct Record ，并且将这些记录再整成一个 vector。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，输出的字段列表。
const vector<struct Record> printRecords(const string, const vector<string>);
// printRecords 函数：根据给定条件查找某数据表内所有记录中符合条件的记录并向屏幕输出数据信息。
// 每一条记录分别整成一个 vector<Record_node>，加上 offset 变成 struct Record ，并且将这些记录再整成一个 vector。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，条件，输出的字段列表，排除范围(即在排除范围内的记录不用判断直接排除)。
const vector<struct Record> printRecords(const string, const vector<Condition>, const vector<string>, const vector<int>);
// printRecords 函数：根据给定条件查找某数据表内的某一范围的记录中符合条件的记录并向屏幕输出数据信息。
// 每一条记录分别整成一个 vector<Record_node>，加上 offset 变成 struct Record ，并且将这些记录再整成一个 vector。
// 输入参数分别为：数据表名(格式为"<数据库名>-<数据表名>")，条件，输出的字段列表，查找范围(即只在对应 offset 的记录中进行逐一判断)，排除范围。
const vector<struct Record> printRecords(const string, const vector<Condition>, const vector<string>, const vector<int>, const vector<int>);
// printNoRecordFound 函数：向屏幕输出查询结果为空。
void printNoRecordFound();
#endif