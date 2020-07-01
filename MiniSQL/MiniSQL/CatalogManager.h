#ifndef _CATALOG_H
#define _CATALOG_H

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <utility>

using namespace std;

// 目前这里只是列出 api 模块中需要用到的 catalog 模块的相关函数即 api 需要的数据的格式。


// databaseExists 函数：判断一个给定名称的数据库是否已经存在。若存在则返回 true，不存在则返回 false。
const bool databaseExists(const string);
// createDatabase 函数：创建一个给定名称的数据库。
void createDatabase(const string);
// dropDatabase 函数：删除一个给定名称的数据库。
void dropDatabase(const string);
// tableExists 函数：判断一个给定名称的数据表是否已经存在。若存在则返回 true，不存在则返回 false。
// 输入参数分别为：数据库名，数据表名。
const bool tableExists(const string, const string);
// createTable 函数：根据给定的字段列表(格式见 api.h 的定义)和主码(若无主码则为"")创建一个数据表。
// 输入参数分别为：数据库名，数据表名，字段列表，主键名称。
void createTable(const string, const string, const vector<struct field>, const string);
// dropTable 函数：删除一个给定名称的数据表。若失败需要输出原因。
// 输入参数分别为：数据库名，数据表名。
void dropTable(const string, const string);
// getFields 函数：获取一个数据表中的所有字段的信息。
// 输入参数分别为：数据库名，数据表名。
const vector<struct field> getFields(const string, const string);
// getField 函数：获取一个数据表中的某个字段的信息。如果字段不存在则将fieldName设置为空。
// 输入参数分别为：数据库名，数据表名，字段名。
const struct field getField(const string, const string, const string);
// isPrimaryKey 函数：判断一个字段是不是主码。
// 输入参数分别为：数据库名，数据表名，字段名。
const bool isPrimaryKey(const string, const string, const string);
// getPrimaryKey 函数：获取一个数据表的主码的字段名。若无主码返回""。
// 输入参数分别为：数据库名，数据表名。
const string getPrimaryKey(const string, const string);
// indexExists 函数：判断一个字段是否已经存在索引(注：本模块中主码自带的索引不包含在内)。
// 输入参数分别为：数据库名，数据表名，字段名。
// const bool indexExists(const string, const string, const string);
// indexExists 函数：判断一个数据库是否已经存在给定名称的索引。
// 输入参数分别为：数据库名，索引名。
const bool indexExists(const string, const string);
// indexInField 函数：获取一个字段的索引。若无索引返回""。
// 输入参数分别为：数据库名，数据表名，字段名。
const string indexInField(const string, const string, const string);
// indiceInTable 函数：获取给定名称的数据表中的所有索引，若无则返回空。
// 输入参数分别为：数据库名，数据表名。
const vector<string> indiceInTable(const string, const string);
// createIndex 函数：创建一个给定名称的索引。
// 输入参数分别为：数据库名，表名，属性名，索引名。
void createIndex(const string D_str, const string T_str, const string A_str, const string I_str);
// dropIndex 函数：删除一个给定名称的索引。
// 输入参数分别为：数据库名，表名，索引名。
void dropIndex(const string D_str,  const string I_str);
// tablesInDatabase 函数：获取给定名称的数据库的所有数据表。
const vector<string> tablesInDatabase(const string);
#endif

