//#include "catalog_manager.h"
//#include "api.h"

#include <vector>
#include <set>
#include <string>

using namespace std;

struct field {
	string fieldName;
	string fieldType;
	int fieldSize;
	bool unique;
};
 
typedef struct Database{
	string name;
	vector <struct Table> tables;
}Database;

vector <Database*> ptrDatabase;					//total pointer vector  to database

//std::set <std::string> DatabaseCatlog;			//store database name
//std::set <std::string> TableCatlog;				//store table name

typedef struct Table{
	string name;
	string key;
	vector <struct field> attributes;
	vector	<vector<struct Datavalue>> values;
}Table;

typedef struct Datavalue{
	//string name;
	char valuetype;					//'f',float,'i',int,'c',char
	char cvalue;
	float fvalue;
	int ivalue;
};


const bool databaseExists(const string str)
{
	//return DatabaseCatlog.find(str)!= DatabaseCatlog.end();
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=str;iter++);
	return iter!= ptrDatabase.end();
}

void createDatabase(const string str)
{
	//if(databaseExists(str)==1) throw SqlError("Database already exists.");
	//DatabaseCatlog.insert(str);
	Database* p = new Database;
	ptrDatabase.push_back(p);
}


void dropDatabase(const string str)
{
	//if(databaseExists(str)==0) throw SqlError("Database not exists.");
	Database* p;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=str;iter++);
	if(iter!=ptrDatabase.end()){
		p = *iter;
		delete p;
		ptrDatabase.erase(iter);											//delete database
	} else{
		//throw SqlError("Database not exists.");
	}	
	//auto tmp = DatabaseCatlog.find(str);
	//if(tmp!=DatabaseCatlog.end()) 
	//	DatabaseCatlog.erase(tmp);											//delete database name in catalog
}

const bool tableExists(const string str, const string str_)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=str;iter++);										//find database
	if(iter!=ptrDatabase.end()){							
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=str_;iter_++);									//find table
		return iter_ != ((*iter)->tables).end();
	} else{
		//throw SqlError("Database not exists.");
		return false;
	}
}

void createTable(const string D_str, const string T_str, const vector<struct field> attr, const string primarykey)
{
	Table tmp;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ == ((*iter)->tables).end()){
			tmp.name = T_str;
			tmp.attributes = attr;
			tmp.key = primarykey;
			//建立一个索引？？？ 
			vector <struct field>::iterator iter_f = tmp.attributes.begin();
			for(;(*iter_f).fieldName != primarykey;iter_f++);								//find primary key
			if(iter_f==tmp.attributes.end()){
				//throw SqlError("primarykey not exists."); 
			} else{
				(*iter_f).unique = true;													//set it unique
			}
		} else{
			//throw SqlError("Table already exists.");
		}
	}else{
		//throw SqlError("Database not exists.");
	}
}

void dropTable(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ != ((*iter)->tables).end()){
			((*iter)->tables).erase(iter_); 
		}else{
			//throw SqlError("Table not exists.");
		}
	}else{
		//throw SqlError("Database not exists.");
	}
}

const vector<struct field> getFields(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){							
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ == ((*iter)->tables).end()){
			//throw SqlError("Table not exists.");
		} else{
			return (*iter_).attributes;
		}
	} else{
		//throw SqlError("Database not exists.");
	}
}

const struct field getField(const string D_str, const string T_str, const string fname)
{
	struct field tmp;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){							
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ == ((*iter)->tables).end()){
			//throw SqlError("Table not exists.");
		} else{
			vector <struct field>::iterator iter_f = ((*iter_).attributes).begin();
			for(;(*iter_f).fieldName!=fname;iter_++);
			if(iter_f == ((*iter_).attributes).end()){
				tmp.fieldName = nullptr;
				return tmp;
			} else{
				return *iter_f;
			}
		}
	} else{
		//throw SqlError("Database not exists.");
	}
}

const bool isPrimaryKey(const string D_str, const string T_str, const string fname)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){							
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ == ((*iter)->tables).end()){
			//throw SqlError("Table not exists.");
		} else{
			return (*iter_).key==fname;
		}
	} else{
		//throw SqlError("Database not exists.");
	}
}

const string getPrimaryKey(const string D_str, const string T_str)
{
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter!=ptrDatabase.end()){							
		vector <struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;(*iter_).name!=T_str;iter_++);
		if(iter_ == ((*iter)->tables).end()){
			//throw SqlError("Table not exists.");
		} else{
			return (*iter_).key;
		}
	} else{
		//throw SqlError("Database not exists.");
	}
}

const bool indexExists(const string D_str, const string I_str)
{

}

const string indexInField(const string D_str, const string T_str, const string I_str)
{
	
}

const vector<string> indiceInTable(const string D_str, const string T_str)
{
	
}

void createIndex(const string D_str, const string I_str)
{
	
}

void dropIndex(const string D_str, const string I_str)
{
	
}

const vector<string> tablesInDatabase(const string D_str)
{
	vector<string> tmp;
	vector <Database*>::iterator iter = ptrDatabase.begin();
	for(;(*iter)->name!=D_str;iter++);
	if(iter==ptrDatabase.end()){
		//throw SqlError("Database not exists.");
	} else{
		vector<struct Table>::iterator iter_ = ((*iter)->tables).begin();
		for(;iter_ != ((*iter)->tables).end();iter++){
			tmp.push_back((*iter_).name);
		}
	} 
	return tmp;
}



