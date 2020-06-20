#ifndef _INDEX_H
#define _INDEX_H

// 目前这里只是列出了 api 模块需要的函数。

#include <string>
#include "minisql.h"
using namespace std;

// initIndex 函数：建立一个给的名称的索引。
// 由于 SQL 不同数据库的索引可以重名，因此这里所以名的格式为"<数据库名>-<索引名>"，可以以此作为索引的名字。
// 输入参数：索引名，类型('d'为整形, 'c'为字符, 'f'为浮点数)，长度。
void initIndex(const string, const char, const int);
// deleteIndex 函数：删除给定名称的索引。
void deleteIndex(const string);
// insertIndexNode 函数：向给定名称的索引中插入值。
// 输入参数：索引名，值，在 record 中的偏移量，[定长字符串的长度]。
void insertIndexNode(const string, const int, const int);
void insertIndexNode(const string, const float, const int);
void insertIndexNode(const string, const FixedString, const int, const int);
// deleteIndexNode 函数：在给定名称的索引中删除某个值。
// 输入参数：索引名，值。
void deleteIndexNode(const string, const int);
void deleteIndexNode(const string, const float);
void deleteIndexNode(const string, const FixedString);
// findIndexNode 函数：寻找索引中某个值在 record 中的偏移量。-1代表找不到。
// 输入参数：索引名，值。
int findIndexNode(const string, const int);
int findIndexNode(const string, const float);
int findIndexNode(const string, const FixedString);
// clearIndex 函数：清空给定名称的索引中的全部值(注意是清空不是删除！)。
void clearIndex(const string);
#endif