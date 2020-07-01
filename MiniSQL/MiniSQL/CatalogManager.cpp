#include "CatalogManager.h"
#include "API.h"
using namespace std;


//struct field {
//	string fieldName;
//	string fieldType;
//	int fieldSize;
//	bool unique;
//};

typedef struct Database {
	string name;
	vector <struct Table> tables;
}Database;

vector <Database*> ptrDatabase;					//total pointer vector  to database

//std::set <std::string> DatabaseCatlog;			//store database name
//std::set <std::string> TableCatlog;				//store table name

typedef struct Table {
	string name;									//table name
	string key;
	string filename;								//the name of the file storing the table(databasename+tablename)
	vector <pair<string, string>> indexs;				//index name, attribute name 
	vector <struct field> attributes;
	//vector	<vector<struct Datavalue>> values;
}Table;

typedef struct Datavalue {
	//string name;
	char valuetype;					//'f',float,'i',int,'c',char
	char cvalue;
	float fvalue;
	int ivalue;
};


const bool databaseExists(const string str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != str; iter++);
	return iter != ptrDatabase.end();
}

void createDatabase(const string str)
{
	//if(databaseExists(str)==1) throw SqlError("Database already exists.");
	Database* p = new Database;
	ptrDatabase.push_back(p);
}


void dropDatabase(const string str)
{
	//if(databaseExists(str)==0) throw SqlError("Database not exists.");
	Database* p;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != str; iter++);
	if (iter != ptrDatabase.end()) {
		p = *iter;
		delete p;
		ptrDatabase.erase(iter);											//delete database
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const bool tableExists(const string str, const string str_)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != str; iter++);										//find database
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != str_; iter_++);									//find table
		return iter_ != ((*iter)->tables).end();
	}
	else {
		throw SqlError("Database not exists.");
		return false;
	}
}

void createTable(const string D_str, const string T_str, const vector<struct field> attr, const string primarykey)
{
	Table tmp;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			tmp.name = T_str;
			tmp.attributes = attr;
			tmp.key = primarykey;
			tmp.filename = D_str + T_str;
			vector <struct field>::iterator iter_f = tmp.attributes.begin();
			for (; (*iter_f).fieldName != primarykey; iter_f++);								//find primary key
			if (iter_f == tmp.attributes.end()) {
				throw SqlError("primarykey not exists.");
			}
			else {
				(*iter_f).unique = true;													//set it unique
			}
		}
		else {
			throw SqlError("Table already exists.");
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

void dropTable(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ != ((*iter)->tables).end()) {
			((*iter)->tables).erase(iter_);
		}
		else {
			throw SqlError("Table not exists.");
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const vector<struct field> getFields(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			return (*iter_).attributes;
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const struct field getField(const string D_str, const string T_str, const string fname)
{
	struct field tmp;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			vector <struct field>::iterator iter_f = ((*iter_).attributes).begin();
			for (; (*iter_f).fieldName != fname; iter_++);
			if (iter_f == ((*iter_).attributes).end()) {
				tmp.fieldName = nullptr;
				return tmp;
			}
			else {
				return *iter_f;
			}
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const bool isPrimaryKey(const string D_str, const string T_str, const string fname)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			return (*iter_).key == fname;
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const string getPrimaryKey(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			return (*iter_).key;
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}


const bool indexExists(const string D_str, const string I_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; iter_ != ((*iter)->tables).end(); iter_++) {
			//vector<string>::iterator iter_I = find((*iter_).indexs.begin(), (*iter_).indexs.end(), I_str);
			vector<pair<string, string>>::iterator iter_I = (*iter_).indexs.begin();
			for (; iter_I != (*iter_).indexs.end(); iter_++) {
				if ((*iter_I).first == I_str) {
					return true;
				}
			}
			continue;
		}
		return false;
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const string indexInField(const string D_str, const string T_str, const string A_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			vector<pair<string, string>>::iterator iter_I = (*iter_).indexs.begin();
			for (; iter_I != (*iter_).indexs.end(); iter_++) {
				if ((*iter_I).second == A_str) {
					return (*iter_I).first;
				}
			}
			return nullptr;
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

const vector<string> indiceInTable(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			vector<pair<string, string>>::iterator iter_I = (*iter_).indexs.begin();
			vector <string> tmp;
			for (; iter_I != (*iter_).indexs.end(); iter_++) {
				tmp.push_back((*iter_I).first);
			}
			return tmp;
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

//接口更改，增加了T_str(数据表名称) ，A_str(索引对应的属性名称) 
void createIndex(const string D_str, const string T_str, const string A_str, const string I_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	if (iter != ptrDatabase.end()) {
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for (; (*iter_).name != T_str; iter_++);
		if (iter_ == ((*iter)->tables).end()) {
			throw SqlError("Table not exists.");
		}
		else {
			pair<string, string> tmp(I_str, A_str);
			(*iter_).indexs.push_back(tmp);
		}
	}
	else {
		throw SqlError("Database not exists.");
	}
}

////接口更改，增加了T_str(数据表名称) 
//void dropIndex(const string D_str, const string T_str, const string I_str)
//{
//	vector <Database*>::iterator iter = ptrDatabase.begin();
//	for (; (*iter)->name != D_str; iter++);
//	if (iter != ptrDatabase.end()) {
//		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
//		for (; (*iter_).name != T_str; iter_++);
//		if (iter_ != ((*iter)->tables).end()) {
//			vector<pair<string, string>>::iterator iter_I = (*iter_).indexs.begin();
//			for (; (*iter_I).first != I_str; iter_I++);
//			if (iter_ == ((*iter)->tables).end()) {
//				throw SqlError("Index not exists.");
//			}
//			else
//				((*iter_).indexs).erase(iter_I);
//		}
//		else {
//			throw SqlError("Table not exists.");
//		}
//	}
//	else {
//		throw SqlError("Database not exists.");
//	}
//}

//整合时修改：dropIndex函数
//由于用户输入的drop index的sql语句中不含表名信息，故仍对所有表进行遍历，参数：数据库名，索引名
//在api中已经（利用indexExists函数）判断过index是否存在，故在此函数中不需要再次判断
void dropIndex(const string D_str, const string I_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for (; (*iter)->name != D_str; iter++);
	vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
	for (; iter_ != ((*iter)->tables).end(); iter_++) {
		vector<pair<string, string>>::iterator iter_I = (*iter_).indexs.begin();
		for (; iter_I != (*iter_).indexs.end(); iter_++) {
			if ((*iter_I).first == I_str) {
				((*iter_).indexs).erase(iter_I);
				return;
			}
		}
	}

}