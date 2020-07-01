#include "API.h"

using namespace std;

/********************
* 注意：目前这段代码没有经过测试，仅供参考，不保证运行正确性。
* 并且，目前只完成了 API 模块的核心函数，如果有其他的模块对接需要，请向我提出。
* API 模块的编写思路参考 miniSQL 大纲和辅导 PPT，思路以这两个文档为主，而不是例程。
*/

// TODO: 查询语句目前没有用上 index，之后要用一下。

static string currentDatabase = "";    // 当前使用的数据库名称。

// executeCommand 函数：API 模块的核心函数。
// 本函数负责将 interpreter 处理得到的命令分给 Catalog Manager, Record Manager 和 Index Manager 进行处理。
// 本函数的输入 cmd 为 struct command，具体的解释见"interpreter.h"文件。
void executeCommand(const struct command cmd)
{
	// 数据库相关操作。Interpreter 只需要保证语法正确，不需要保证数据库的存在与否。
	if (cmd.op == "00")
	{
		// 创建数据库，cmd 的 op 为"00"。
		// arg1 为数据库的名字，interpreter 需要保证其不能为空且符合命名规范，其余成员为空。
		// (不过话说其实咱们的大程大纲里没要求数据库，但是大程辅导ppt里面是有的，因此先写了再说)
		if (databaseExists(cmd.arg1))
		{
			throw SqlError("Database already exists.");
		}
		createDatabase(cmd.arg1);
	}
	else if (cmd.op == "01")
	{
		// 使用指定的数据库，cmd 的 op 为"01"。
		// arg1 为数据库的名字，interpreter 需要保证其不能为空，其余成员为空。
		if (!databaseExists(cmd.arg1))
		{
			throw SqlError("Database does not exist.");
		}
		currentDatabase = cmd.arg1;
	}
	else if (cmd.op == "02")
	{
		// 删除数据库，cmd 的 op 为"02"。
		// arg1 为数据库的名字，interpreter 需要保证其不能为空，其余成员为空。
		if (!databaseExists(cmd.arg1))
		{
			throw SqlError("Database does not exist.");
		}
		vector<string> tableList = tablesInDatabase(cmd.arg1);
		// 删除数据表及数据表中的索引。
		for (unsigned int i = 0; i < tableList.size(); i++)
		{
			vector<string> indice = indiceInTable(currentDatabase, tableList[i]);
			for (unsigned int j = 0; j < indice.size(); j++)
			{
				deleteIndex(currentDatabase + "-" + indice[j]);
			}
			deleteTable(currentDatabase + "-" + tableList[i]);
		}
		dropDatabase(cmd.arg1);
		if (currentDatabase == cmd.arg1)
		{
			currentDatabase = "";
		}
	}
	else
	{
		if (currentDatabase == "")
		{
			throw SqlError("No database selected.");
		}
		// 数据表相关操作。Interpreter 只需要保证语法正确，不需要保证数据表的存在与否。
		if (cmd.op == "10")
		{
			// 创建数据表，cmd 的 op 为"10"。
			// arg1 为数据表的名字（必须符合命名规范）；arg2 为语句中"(...)"内的内容(不包括括号本身，但是不能有换行！)，arg2【不需要】保证语法正确，api 模块会进行检查和分析。
			if (tableExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Table already exists.");
			}
			vector<struct field> fields; // 最终生成的各个字段，具体的解释见"catalog.h"文件。
			string primaryKey = ""; // 数据表的主码。
			tableCreateProcess(cmd.arg2, fields, primaryKey); // 处理 arg2. 因为实在内容太复杂因此单独列为一个函数。
			createTable(currentDatabase, cmd.arg1, fields, primaryKey);
			initTable(currentDatabase + "-" + cmd.arg1, fields);
			// 对主码自动创建一个索引。为了防止重名，索引的名称为"<数据库名>#<数据表名>"。
			// 对主码创建的索引不用 catalog manager 单独管理。
			if (primaryKey != "")
			{
				unsigned int i = 0;
				for (i = 0; i < fields.size(); i++)
				{
					if (fields[i].fieldName == primaryKey)
					{
						break;
					}
				}
				if (fields[i].fieldType == "int")
				{
					initIndex(currentDatabase + "#" + cmd.arg1, 'd', sizeof(int));
				}
				else if (fields[i].fieldType == "char")
				{
					initIndex(currentDatabase + "#" + cmd.arg1, 'c', fields[i].fieldSize);
				}
				else if (fields[i].fieldType == "float")
				{
					initIndex(currentDatabase + "#" + cmd.arg1, 'f', sizeof(float));
				}
			}
		}
		else if (cmd.op == "11")
		{
			// 删除数据表，cmd 的 op 为"11"。
			// arg1 为数据表的名字，interpreter 需要保证其不能为空。本模块不保证数据表能否被删除，只能保证被删除的数据表存在。
			if (!tableExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Table does not exist.");
			}
			// 删除数据表及数据表中的索引。
			vector<string> indice = indiceInTable(currentDatabase, cmd.arg1);
			for (unsigned int i = 0; i < indice.size(); i++)
			{
				deleteIndex(currentDatabase + "-" + indice[i]);
			}
			// 删除数据表的主键索引。
			const string primaryKey = getPrimaryKey(currentDatabase, cmd.arg1);
			if (primaryKey != "")
			{
				deleteIndex(currentDatabase + "#" + cmd.arg1);
			}
			deleteTable(currentDatabase + "-" + cmd.arg1);
			dropTable(currentDatabase, cmd.arg1);
		}
		// 索引相关操作。Interpreter 只需要保证语法正确，不需要保证索引的存在与否。
		else if (cmd.op == "20")
		{
			// 创建索引，cmd 的 op 为"20"。
			// arg1 为索引的名字，arg2 为表名，arg3 为字段名。
			if (!tableExists(currentDatabase, cmd.arg2))
			{
				throw SqlError("Table doesn't exist.");
			}
			if (indexInField(currentDatabase, cmd.arg2, cmd.arg3) != "")
			{
				throw SqlError("Index already created on target field.");
			}
			if (isPrimaryKey(currentDatabase, cmd.arg2, cmd.arg3))
			{
				throw SqlError("Target field is a primary key, thus index has been automaticly created.");
			}
			if (indexExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Index with given name already exists in the database.");
			}
			const struct field fieldInfo = getField(currentDatabase, cmd.arg2, cmd.arg3); // 要建索引的字段的具体信息。
			if (!fieldInfo.unique) // miniSQL 中的要求为索引是单值索引。
			{
				throw SqlError("Index should be created on unique field for miniSQL.");
			}
			else
			{
				createIndex(currentDatabase, cmd.arg2, cmd.arg3, cmd.arg1);
				// 建立索引并插入初始值。
				if (fieldInfo.fieldType == "int")
				{
					initIndex(currentDatabase + "-" + cmd.arg1, 'd', sizeof(int));
					const vector<Value_node> records = valuesInField(currentDatabase, cmd.arg2 + "-" + cmd.arg3);
					for (Value_node record : records)
					{
						insertIndexNode(currentDatabase + "-" + cmd.arg1, record.data.d, record.offset);
					}
				}
				else if (fieldInfo.fieldType == "float")
				{
					initIndex(currentDatabase + "-" + cmd.arg1, 'f', sizeof(float));
					const vector<Value_node> records = valuesInField(currentDatabase, cmd.arg2 + "-" + cmd.arg3);
					for (Value_node record : records)
					{
						insertIndexNode(currentDatabase + "-" + cmd.arg1, record.data.f, record.offset);
					}
				}
				else if (fieldInfo.fieldType == "char")
				{
					initIndex(currentDatabase + "-" + cmd.arg1, 'c', fieldInfo.fieldSize);
					const vector<Value_node> records = valuesInField(currentDatabase, cmd.arg2 + "-" + cmd.arg3);
					for (Value_node record : records)
					{
						insertIndexNode(currentDatabase + "-" + cmd.arg1, record.data.c, fieldInfo.fieldSize, record.offset);
					}
				}
			}
		}
		else if (cmd.op == "21")
		{
			// 删除索引，cmd 的 op 为"21"。
			// arg1 为索引的名字。
			if (!indexExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Index doesn't exist.");
			}
			dropIndex(currentDatabase, cmd.arg1);
			deleteIndex(currentDatabase + "-" + cmd.arg1);
		}
		else if (cmd.op == "30")
		{
			// 插入记录，cmd 的 op 为"30"。一次只能插入一条记录。
			// arg1 为数据表的名字。
			// arg2 为语句中"(...)"内的内容(不包括括号本身，但是不能有换行！)
			// 因为 miniSQL 不允许空值，因此插入只能插入全部字段。
			if (!tableExists(currentDatabase, cmd.arg1)) //如果数据表不存在 
			{
				throw SqlError("Table doesn't exist.");
			}
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // 要插入的数据表的字段信息。
			vector<Record_node> record; // 要插入的记录的信息。
			recordInsertProcess(cmd.arg1, cmd.arg2, fields, record);	// 处理 arg2. 因为实在内容太复杂因此单独列为一个函数。
			// 插入数据。
			const int offset = insertRecord(currentDatabase + "-" + cmd.arg1, record);
			// 判断数据表是否有索引，如果有则插入对应数据。
			vector<string> indice;
			for (unsigned int i = 0; i < fields.size(); i++)
			{
				string index = "";	// 要插入数据的索引的全称。
				string indexName = indexInField(currentDatabase, cmd.arg1, fields[i].fieldName);
				if (indexName != "")
				{
					index = currentDatabase + "-" + indexName;
				}
				else if (isPrimaryKey(currentDatabase, cmd.arg1, fields[i].fieldName))
				{
					index = currentDatabase + "#" + cmd.arg1;
				}
				if (index != "")
				{
					if (record[i].type == 'd')
					{
						insertIndexNode(index, record[i].data.d, offset);
					}
					else if (record[i].type == 'f')
					{
						insertIndexNode(index, record[i].data.f, offset);
					}
					else if (record[i].type == 'c')
					{
						insertIndexNode(index, record[i].data.c, offset, record[i].size);
					}
				}
			}
		}
		else if (cmd.op == "31")
		{
			// 删除记录(无 where 子句)，cmd 的 op 为"31"。
			// arg1 为数据表的名字。
			if (!tableExists(currentDatabase, cmd.arg1)) //如果数据表不存在 
			{
				throw SqlError("Table doesn't exist.");
			}
			// 判断该数据表是否有索引，如果有则清空它们。
			string primaryKey = getPrimaryKey(currentDatabase, cmd.arg1);
			if (primaryKey != "")
			{
				clearIndex(currentDatabase + "#" + cmd.arg1);
			}
			vector<string> indice = indiceInTable(currentDatabase, cmd.arg1);
			for (string index : indice)
			{
				clearIndex(currentDatabase + "-" + index);
			}
			// 清空数据表。
			clearRecord(currentDatabase + "-" + cmd.arg1);
		}
		else if (cmd.op == "32")
		{
			// 删除记录(有 where 子句)，cmd 的 op 为"32"。
			// arg1 为数据表的名字。
			// arg2 为 where 子句中的内容(不含"where"关键词和分号)。
			if (!tableExists(currentDatabase, cmd.arg1)) //如果数据表不存在 
			{
				throw SqlError("Table doesn't exist.");
			}
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // 要处理的数据表的字段信息。
			vector<Condition> conditions;
			try		// 跳过肯定不能匹配的情形。
			{
				conditionProcess(cmd.arg2, fields, conditions);
				struct recordRange recordRange = recordProcess(cmd.arg1, conditions);
				vector<struct Record> results;
				// range 里排除 excluded。
				if (!recordRange.norange)
				{
					recordRange.range = excludeOffsets(recordRange.range, recordRange.excluded);
					results = findRecords(currentDatabase + "-" + cmd.arg1, conditions, recordRange.range, recordRange.excluded);
				}
				else
				{
					results = findRecords(currentDatabase + "-" + cmd.arg1, conditions, recordRange.excluded);
				}
				// 选出存在 index 的字段并记录其字段名。
				vector<string> indice;
				for (const struct field field : fields)
				{
					string indexName = indexInField(currentDatabase, cmd.arg1, field.fieldName);
					if (indexName != "")
					{
						indice.push_back(currentDatabase + "-" + indexName);
					}
					else if (isPrimaryKey(currentDatabase, cmd.arg1, field.fieldName))
					{
						indice.push_back(currentDatabase + "#" + cmd.arg1);
					}
					else
					{
						indice.push_back("");
					}
				}
				for (unsigned int i = 0; i < results.size(); i++)
				{
					const vector<Record_node> result = results[i].list;
					for (unsigned int j = 0; j < result.size(); j++)
					{
						if (indice[j] != "")
						{
							if (result[j].type == 'd')
							{
								deleteIndexNode(indice[j], result[j].data.d);
							}
							else if (result[j].type == 'f')
							{
								deleteIndexNode(indice[j], result[j].data.f);
							}
							else if (result[j].type == 'c')
							{
								deleteIndexNode(indice[j], result[j].data.c);
							}
						}
					}
					deleteRecord(currentDatabase + "-" + cmd.arg1, results[i].offset);
				}
			}
			catch (DoNotMatch) {};
		}
		else if (cmd.op == "33")
		{
			// 选择记录（无 where 子句），cmd 的 op 为"33"。
			// arg1 为选择的字段("*"代表全部)。
			// arg2 为选择的数据表。
			if (!tableExists(currentDatabase, cmd.arg2)) //如果数据表不存在 
			{
				throw SqlError("Table doesn't exist.");
			}
			vector<string> fieldList;
			fieldsProcess(cmd.arg2, cmd.arg1, fieldList);
			printRecords(currentDatabase + "-" + cmd.arg2, fieldList);
		}
		else if (cmd.op == "34")
		{
			// 选择记录（有 where 子句），cmd 的 op 为"33"。
			// arg1 为选择的字段("*"代表全部)。
			// arg2 为选择的数据表。
			// arg3 为 where 子句中的内容(不含"where"关键词和分号)。
			if (!tableExists(currentDatabase, cmd.arg2)) //如果数据表不存在 
			{
				throw SqlError("Table doesn't exist.");
			}
			vector<string> fieldList;
			fieldsProcess(cmd.arg2, cmd.arg1, fieldList);
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // 要处理的数据表的字段信息。
			vector<Condition> conditions;
			try		// 跳过肯定不能匹配的情形。
			{
				conditionProcess(cmd.arg3, fields, conditions);
				struct recordRange recordRange = recordProcess(cmd.arg2, conditions);
				if (!recordRange.norange)
				{
					recordRange.range = excludeOffsets(recordRange.range, recordRange.excluded);
					printRecords(currentDatabase + "-" + cmd.arg2, conditions, fieldList, recordRange.range, recordRange.excluded);
				}
				else
				{
					printRecords(currentDatabase + "-" + cmd.arg2, conditions, fieldList, recordRange.excluded);
				}
			}
			catch (DoNotMatch) {
				printNoRecordFound();
			};
		}
	}
};

// inCondInterval 函数：判断一个数值是否在条件的区间内。
bool inCondInterval(float value, const Condition cond)
{
	if (cond.op == EQ)
	{
		if (value == cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == DIFF)
	{
		if (value != cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == LT)
	{
		if (value < cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == GT)
	{
		if (value > cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == LTE)
	{
		if (value <= cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == GTE)
	{
		if (value >= cond.getValue1())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (cond.op == BETW)
	{
		if ((value <= cond.getValue1() && cond.eq1 == false) || (value < cond.getValue1() && cond.eq1 == true)
			|| (value >= cond.getValue2() && cond.eq2 == false) || (value > cond.getValue2() && cond.eq2 == true))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

// 以下的内容是 API 模块处理一些有的没的玩意，仅供自用，如果你对这些不感兴趣可以不用看下去了。
// 之后可能会把这些函数单独塞一个文件里边。

// tableCreateProcess 函数：处理创建数据表时的参数。
// 输入：
// info 为创建数据表语句中对数据表内容定义的字符串。
// 输出：
// fields 为生成的数据表各字段信息。
// primaryKey 为生成的数据表的主码。
void tableCreateProcess(const string info, vector<struct field>& fields, string &primaryKey)
{
	int lpos = 0, rpos = 0; // lpos 和 rpos 用于将一大坨语句按照逗号分段，便于进一步处理。
	string line; // 要处理的语句分段。
	do {
		rpos = info.find(',', lpos);
		if (rpos != string::npos)
		{
			line = info.substr(lpos, rpos - lpos);
		}
		else
		{
			line = info.substr(lpos, info.length() - lpos);
		}
		lpos = rpos + 1;

		int state = 0; // 判断当前处理的状态。
		int wpos = 0; // 当前单词的左界。
		vector<string> words;
		unsigned int i;
		// 开始分析这坨玩意。
		for (i = 0; i < line.length(); i++)
		{
			if (state == 0) // 读取空格。
			{
				if (line[i] == '(') // 读取的为参数的情况。
				{
					wpos = i + 1;
					state = 2;
				}
				else if (line[i] != ' ') // 开始读单词。
				{
					state = 1;
					wpos = i;
				}
			}
			else if (state == 1) // 读取单词。
			{
				if (line[i] == '(') // 读取的为参数的情况。
				{
					words.push_back(line.substr(wpos, i - wpos));
					wpos = i + 1;
					state = 2;
				}
				else if (line[i] == ' ') // 单词读取完毕，此时开始处理单词。
				{
					words.push_back(line.substr(wpos, i - wpos));
					state = 0;
				}
			}
			else if (state == 2) // 读取参数。
			{
				if (line[i] == ')') // 参数读取完毕。
				{
					string param;
					param = line.substr(wpos, i - wpos);
					size_t testa = param.find_first_not_of(' ');
					size_t testb = param.find_last_not_of(' ');
					param = param.substr(param.find_first_not_of(' '), param.find_last_not_of(' ') + 1);
					param = "(" + param + ")"; // 将参数进行格式化。
					words.push_back(param);
					state = 0;
				}
			}
		}
		if (state == 1)
		{
			words.push_back(line.substr(wpos, i - wpos));
		}
		if (state == 2)
		{
			throw SqlError("Missing closed bracket.");
		}
		// 判断语义。
		if (words.size() >= 3 && words[0] == "primary" && words[1] == "key" \
			&& words[2][0] == '(' && words[2][words[2].length() - 1] == ')')
		{
			// 如果是主码。
			if (primaryKey != "")  // 根据题目要求，miniSQL 不支持多主码的情况。
			{
				throw SqlError("Multiple primary keys. This feature is not supported in miniSQL.");
			}
			for (unsigned int i = 0; i < fields.size(); i++)  // SQL 要求主码的字段必须在前面已经定义。
			{
				if (fields[i].fieldName == words[2].substr(1, words[2].length() - 2))
				{
					primaryKey = fields[i].fieldName;
					fields[i].unique = true;	// 主码强制 unique。
				}
			}
			if (primaryKey == "")  // 主码无效的情况。
			{
				throw SqlError("Field undefined for primary key definition.");
			}
		}
		else if (words.size() < 2)
		{
			// 如果没有表明类型。
			throw SqlError("Undefined field type.");
		}
		else
		{
			string fieldName, fieldType; // 分析得到的字段信息。
			unsigned int fieldSize = 0; // 分析得到的字段长度。
			bool unique = false; // 分析得到的字段是否唯一。
			bool flag = false; // 判断字段名是否有效。SQL 中要求字段命名必须为字母、数字或下划线，无首字符不得为数字的要求。
			for (char c : words[0])
			{
				if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'))
				{
					flag = true;
				}
			}
			if (flag)
			{
				throw SqlError("Invalid field name \"" + words[0] + "\".");
			}
			if (words[1] == "char")
			{
				if (words.size() > 4 || words.size() < 3 || (words.size() == 4 && words[3] != "unique")) // 如果格式不正确。
				{
					throw SqlError("Invalid syntax.");
				}
				bool flag = false; // 判断数字参数内是否有非数字字符。
				for (unsigned int i = 1; words[2][i] != ')' && i < words[2].length(); i++)
				{
					if (words[2][i] < '0' || words[2][1] > '9')
					{
						flag = true;
					}
				}
				if (flag) // 如果有非数字字符。
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should be numeric.");
				}
				try
				{
					fieldSize = stoi(words[2].substr(1, words[2].length() - 2));
				}
				catch (std::out_of_range) // 如果出现越界的错误。
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should between 1 and 255.");
				}
				if (fieldSize <= 0 || fieldSize > 255) // miniSQL 要求字符型字段的长度在不大于255.
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should between 1 and 255.");
				}
				if (words.size() == 4 && words[3] == "unique")
				{
					unique = true;
				}
				if (fields.size() >= 32) // 如果字段太多
				{
					throw SqlError("Too many fields for a table. No more than 32 fields is allowed for miniSQL.");
				}
				fieldName = words[0];
				fieldType = words[1];
				const struct field targetField = { fieldName, fieldType, fieldSize, unique };
				fields.push_back(targetField);
			}
			else if (words[1] != "int" && words[1] != "float")
			{
				throw SqlError("Invalid field type for field \"" + words[0] + "\".");
			}
			else
			{
				if (words.size() > 3 || words.size() < 2 || (words.size() == 3 && words[2] != "unique")) // 如果格式不正确。
				{
					throw SqlError("Invalid syntax.");
				}
				if (fields.size() >= 32) // 如果字段太多
				{
					throw SqlError("Too many fields for a table. No more than 32 fields is allowed for miniSQL.");
				}
				if (words.size() == 3 && words[3] == "unique")
				{
					unique = true;
				}
				fieldName = words[0];
				fieldType = words[1];
				if (words[1] == "int")
				{
					fieldSize = sizeof(int);
				}
				else if (words[1] == "float")
				{
					fieldSize = sizeof(float);
				}
				const struct field targetField = { fieldName, fieldType, fieldSize, unique };
				fields.push_back(targetField);
			}
			// 判断字段是否重名
			for (unsigned int i = 0; i < fields.size() - 1; i++)
			{
				if (fields[i].fieldName == fields[fields.size() - 1].fieldName)
				{
					throw SqlError("Multiple field name \"" + fields[i].fieldName + "\".");
				}
			}
		}
	} while (rpos != string::npos);
}
// recordInsertProcess 函数：处理插入数据的函数。
// 输入：
// table 为数据表名。
// info 为插入数据表的值的信息。
// fields 为被插入的表的字段信息。
// 输出：
// record 为要插入的记录的信息。
void recordInsertProcess(const string table, const string info, const vector<struct field> fields, vector<Record_node> &record)
{
	int lpos = 0, rpos = 0; // lpos 和 rpos 用于将一大坨语句按照逗号分段，便于进一步处理。
	vector<string> values;  // 各个要插入的值。
	do {
		string line; // 要处理的语句分段。
		rpos = info.find(',', lpos);
		if (rpos != string::npos)
		{
			line = info.substr(lpos, rpos - lpos);
		}
		else
		{
			line = info.substr(lpos, info.length() - lpos);
		}
		lpos = rpos + 1;
		line = line.substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
		values.push_back(line);
	} while (rpos != string::npos);
	if (values.size() != fields.size())
	{
		throw SqlError("Number of values to insert does not match number of fields of target table.");
	}
	for (unsigned int i = 0; i < values.size(); i++)
	{
		if (fields[i].fieldType == "int")
		{
			for (unsigned int j = 0; j < values[i].length(); j++)
			{
				if (values[i][j] <'0' || values[i][j] > '9') // 如果为非数字
				{
					throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" does not match the type \"int\".");
				}
			}
			int value;
			try
			{
				value = stoi(values[i]);
			}
			catch (out_of_range) // 如果出现越界的错误。
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" out of range.");
			}
			if (isPrimaryKey(currentDatabase, table, fields[i].fieldName))
			{
				// 主码的 unique 测试。
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique 测试。
				string indexName = indexInField(currentDatabase, table, fields[i].fieldName);
				if (indexName != "")
				{
					if (findIndexNode(currentDatabase + "-" + indexName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
				else
				{
					if (getUniqueOffset(currentDatabase + "-" + table, fields[i].fieldName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
			}
			Record_node n = Record_node(fields[i].fieldName, value);
			record.push_back(n);
		}
		else if (fields[i].fieldType == "float")
		{
			for (unsigned int j = 0; j < values[i].length(); j++)
			{
				int flag;	// 是否已经出现过一次小数点。
				if (values[i][j] <'0' || values[i][j] > '9' || (values[i][j] == '.' && (flag || j == 0 || j == values[i].length() - 1))) // 如果为非数字或小数点位置不对
				{
					throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" does not match the type \"float\".");
				}
				if (values[i][j] == '.')
				{
					flag = true;
				}
			}
			float value;
			try
			{
				value = stof(values[i]);
			}
			catch (out_of_range) // 如果出现越界的错误。
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" out of range.");
			}
			if (isPrimaryKey(currentDatabase, table, fields[i].fieldName))
			{
				// 主码的 unique 测试。
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique 测试。
				string indexName = indexInField(currentDatabase, table, fields[i].fieldName);
				if (indexName != "")
				{
					if (findIndexNode(currentDatabase + "-" + indexName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
				else
				{
					if (getUniqueOffset(currentDatabase + "-" + table, fields[i].fieldName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
			}
			Record_node n = Record_node(fields[i].fieldName, value);
			record.push_back(n);
		}
		else if (fields[i].fieldType == "char")
		{
			if (values[i][0] != '\'' || values[i][values[i].length() - 1] != '\'')
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" does not match the type \"char\".");
			}
			if (values[i].size() > fields[i].fieldSize + 2)
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" is too long.");
			}
			FixedString value = FixedString(fields[i].fieldSize, values[i].substr(1, values[i].length() - 2));
			if (isPrimaryKey(currentDatabase, table, fields[i].fieldName))
			{
				// 主码的 unique 测试。
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique 测试。
				string indexName = indexInField(currentDatabase, table, fields[i].fieldName);
				if (indexName != "")
				{
					if (findIndexNode(currentDatabase + "-" + indexName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
				else
				{
					if (getUniqueOffset(currentDatabase + "-" + table, fields[i].fieldName, value) != -1)
					{
						throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
					}
				}
			}
			Record_node n = Record_node(fields[i].fieldName, value);
			record.push_back(n);
		}
	}
}

// conditionProcess 函数：处理 where 子句内的条件。
// 输入：
// info 为 where 子句内的条件表达式。
// fields 为查询的表的字段信息。(糙，还好大概 miniSQL 一次只处理一张表，没有笛卡尔积，要不然我觉得我和写 record manager 的老哥可以当场跳下去了...)
// 输出：
// conditions 为处理完毕的条件的信息。
void conditionProcess(const string info, const vector<struct field> fields, vector<Condition> &conditions)
{
	int lpos = 0, rpos = 0; // lpos 和 rpos 用于将一大坨语句按照逗号分段，便于进一步处理。
	vector<string> lines;  // 各个要插入的值。
	do {
		string line; // 要处理的语句分段。
		rpos = info.find("and", lpos);	// miniSQL 关键字要求全小写。还好条件只有 and...
		if (rpos != string::npos)
		{
			line = info.substr(lpos, rpos - lpos);
		}
		else
		{
			line = info.substr(lpos, info.length() - lpos);
		}
		lpos = rpos + 3;
		line = line.substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
		lines.push_back(line);
	} while (rpos != string::npos);
	for (string line : lines)
	{
		int oppos;	// 操作符的位置。
		condop op;	// 操作符的类型。
		string attr;		// 要操作的属性。
		string valueStr;	// 要操作的值的字符串形式。
		try
		{
			if ((oppos = line.find("<=")) != string::npos)
			{
				op = LTE;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 2, line.length() - oppos - 2).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else if ((oppos = line.find(">=")) != string::npos)
			{
				op = GTE;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 2, line.length() - oppos - 2).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else if ((oppos = line.find("<>")) != string::npos)
			{
				op = DIFF;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 2, line.length() - oppos - 2).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else if ((oppos = line.find("<")) != string::npos)
			{
				op = LT;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 1, line.length() - oppos - 1).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else if ((oppos = line.find(">")) != string::npos)
			{
				op = GT;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 1, line.length() - oppos - 1).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else if ((oppos = line.find("=")) != string::npos)
			{
				op = EQ;
				attr = line.substr(0, oppos).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
				valueStr = line.substr(oppos + 1, line.length() - oppos - 1).substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			}
			else
			{
				throw SqlError("Invalid syntax for where clause.");
			}
		}
		catch (out_of_range)	// attr 或者 value 的某一方为空
		{
			throw SqlError("Invalid syntax for where clause.");
		}
		int fieldId = -1;	// attr 所指向的字段的 index.
		for (unsigned int j = 0; j < fields.size(); j++)
		{
			if (fields[j].fieldName == attr)
			{
				fieldId = j;
				break;
			}
		}
		if (fieldId == -1)
		{
			throw SqlError("Field \"" + attr + "\" does not exist.");
		}
		// 开始判断类型是否正确。
		if (fields[fieldId].fieldType == "int")
		{
			for (unsigned int j = 0; j < valueStr.length(); j++)
			{
				if (valueStr[j] <'0' || valueStr[j] > '9') // 如果为非数字
				{
					throw SqlError("Value " + valueStr + " for field \"" + attr + "\" does not match the type \"int\".");
				}
			}
			int value;
			try
			{
				value = stoi(valueStr);
			}
			catch (out_of_range) // 如果出现越界的错误。
			{
				throw SqlError("Value " + valueStr + " for field \"" + attr + "\" out of range.");
			}
			Condition cond = Condition(attr, op, value);
			conditions.push_back(cond);
		}
		else if (fields[fieldId].fieldType == "float")
		{
			for (unsigned int j = 0; j < valueStr.length(); j++)
			{
				int flag;	// 是否已经出现过一次小数点。
				if (valueStr[j] <'0' || valueStr[j] > '9' || (valueStr[j] == '.' && (flag || j == 0 || j == valueStr.length() - 1))) // 如果为非数字或小数点位置不对
				{
					throw SqlError("Value " + valueStr + " for field \"" + attr + "\" does not match the type \"float\".");
				}
				if (valueStr[j] == '.')
				{
					flag = true;
				}
			}
			float value;
			try
			{
				value = stof(valueStr);
			}
			catch (out_of_range) // 如果出现越界的错误。
			{
				throw SqlError("Value " + valueStr + " for field \"" + attr + "\" out of range.");
			}
			Condition cond = Condition(attr, op, value);
			conditions.push_back(cond);
		}
		else if (fields[fieldId].fieldType == "char")
		{
			if (valueStr[0] != '\'' || valueStr[valueStr.length() - 1] != '\'')
			{
				throw SqlError("Value " + valueStr + " for field \"" + attr + "\" does not match the type \"char\".");
			}
			if (valueStr.size() > fields[fieldId].fieldSize + 2)
			{
				throw DoNotMatch();
			}
			if (op != EQ && op != DIFF)
			{
				throw SqlError("Invalid operation for field \"" + attr + "\" of type \"char\".");
			}
			FixedString value = FixedString(fields[fieldId].fieldSize, valueStr.substr(1, valueStr.length() - 2));
			Condition cond = Condition(attr, op, value);
			conditions.push_back(cond);
		}
		Condition cond1 = conditions[conditions.size() - 1];	// 最近插入的条件。
		Condition cond2;										// 要比较的条件。
		// 判断是否可以合并条件。
		bool flag = false;	// 为 true 则表明可以进行条件合并
		int i;
		for (i = 0; i < conditions.size(); i++)	// 之所以不用迭代器是因为判断是否为最后一项比这麻烦。
		{
			cond2 = conditions[i];
			if (cond2.attr == cond1.attr)
			{
				flag = true;
				break;	//条件一致则进一步判断
			}
		}
		if (flag)
		{
			if (cond1.type == 'c')
			{
				// 对字符串进行比较
				if (cond1.op == EQ && cond2.op == EQ)
				{
					// 两个均为等于条件，此时值一致则一定同时满足，不一致则不成立。
					if (cond1.value1.c == cond2.value1.c)
					{
						conditions.pop_back();	// 删除一个重复条件。
					}
					else
					{
						throw DoNotMatch();
					}
				}
				else
				{
					// 一个为等于一个为不等于，此时值一致则不成立，不一致取等于的情况。
					if (cond1.value1.c == cond2.value1.c)
					{
						throw DoNotMatch();
					}
					else
					{
						if (cond1.op == EQ)
						{
							conditions.erase(conditions.begin() + i);	// 删除 cond2。
							i--;
						}
						else
						{
							conditions.pop_back();	// 删除 cond1。
						}
					}
				}
			}
			else
			{
				// 对数值进行比较，情况更为复杂一些。
				if (cond1.op == EQ)
				{
					// 第一个条件为等于，那么只有两种情况：第一个条件值在/不在第二个条件的区间内。
					if (inCondInterval(cond1.getValue1(), cond2))
					{
						conditions.erase(conditions.begin() + i);		// 值在区间内，舍弃 cond2。
						i--;
					}
					else
					{
						throw DoNotMatch();			// 值不在区间内，一定不成立。
					}
				}
				else if (cond2.op == EQ)
				{
					// 第一个条件为等于，那么只有两种情况：第一个条件值在/不在第二个条件的区间内。
					if (inCondInterval(cond2.getValue1(), cond1))
					{
						conditions.pop_back();		// 值在区间内，舍弃 cond1。
					}
					else
					{
						throw DoNotMatch();			// 值不在区间内，一定不成立。
					}
				}
				else if (cond1.op == DIFF && !inCondInterval(cond1.getValue1(), cond2))
				{
					// 第一个条件为不等于且值不在第二个条件的区间内，即条件无效，舍弃 cond1。
					conditions.pop_back();
				}
				else if (cond2.op == DIFF && !inCondInterval(cond2.getValue1(), cond1))
				{
					// 第一个条件为不等于且值不在第二个条件的区间内，即条件无效，舍弃 cond2。
					conditions.erase(conditions.begin() + i);
					i--;
				}
				else if (cond2.op == BETW)
				{
					// 第二个条件为两个值之间，那么对第一个条件的值进行判断。第一个条件新插入，不可能为"BETW"（这个条件是合并条件后生成的）。
					if ((cond1.getValue1() <= cond2.getValue1() && cond2.eq1 == false) \
						|| (cond1.getValue1() < cond2.getValue1() && cond2.eq1 == true))
					{
						// 插入的条件比区间左值小。
						if (cond1.op == LT || cond1.op == LTE)
						{
							// 此时条件不可能成立。
							throw DoNotMatch();
						}
						else
						{
							conditions.pop_back();	// 此时 cond1 不起作用。
						}
					}
					else if ((cond1.getValue1() >= cond2.getValue2() && cond2.eq2 == false) \
						|| (cond1.getValue1() > cond2.getValue2() && cond2.eq2 == true))
					{
						// 插入的条件比区间右值大。
						if (cond1.op == GT || cond1.op == GTE)
						{
							// 此时条件不可能成立。
							throw DoNotMatch();
						}
						else
						{
							conditions.pop_back();	// 此时 cond1 不起作用。
						}
					}
					else
					{
						// cond1 的值在 cond2 区间内，此时对 cond2 的范围进行一些调整。
						if (cond1.op == LT)
						{
							cond2.setValue2(cond1.getValue1());
							cond2.eq2 = false;
						}
						else if (cond1.op == LTE)
						{
							cond2.setValue2(cond1.getValue1());
							cond2.eq2 = true;
						}
						else if (cond1.op == GT)
						{
							cond2.setValue1(cond1.getValue1());
							cond2.eq1 = false;
						}
						else if (cond1.op == GTE)
						{
							cond2.setValue2(cond1.getValue1());
							cond2.eq1 = true;
						}
					}
				}
				else
				{
					// cond1 和 cond2 都为大于小于类的条件。
					if (cond1.getValue1() < cond1.getValue1())
					{
						// cond1 的值比 cond2 小。
						if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == LTE || cond2.op == LT))
						{
							// 舍去 cond2 的情形。
							conditions.erase(conditions.begin() + i);
							i--;
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == GTE || cond2.op == GT))
						{
							// 舍去 cond1 的情形。
							conditions.pop_back();
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == LTE || cond2.op == LT))
						{
							// 取区间的情形。
							conditions.erase(conditions.begin() + i);
							i--;
							conditions.pop_back();
							if (cond1.type == 'd')
							{
								Condition cond3 = Condition(cond1.attr, cond1.value1.d, cond1.op == GTE, cond2.value1.d, cond2.op == LTE);
								conditions.push_back(cond3);
							}
							else if (cond1.type == 'f')
							{
								Condition cond3 = Condition(cond1.attr, cond1.value1.f, cond1.op == GTE, cond2.value1.f, cond2.op == LTE);
								conditions.push_back(cond3);
							}
						}
						else if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == GTE || cond2.op == GT))
						{
							// 条件不可能成立。
							throw DoNotMatch();
						}
					}
					else if (cond1.getValue1() == cond2.getValue1())
					{
						// 两个值相等。
						if ((cond1.op == LTE || cond1.op == GTE) && (cond2.op == LTE || cond2.op == GTE))
						{
							// 条件可以合并成相等。
							conditions.erase(conditions.begin() + i);
							i--;
							conditions.pop_back();
							if (cond1.type == 'd')
							{
								Condition cond3 = Condition(cond1.attr, EQ, cond1.value1.d);
							}
							else if (cond1.type == 'f')
							{
								Condition cond3 = Condition(cond1.attr, EQ, cond1.value1.f);
							}
						}
						else
						{
							// 条件不可能成立。
							throw DoNotMatch();
						}
					}
					else if (cond1.getValue1() > cond2.getValue1())
					{
						// cond1 的值比 cond2 大。
						if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == LTE || cond2.op == LT))
						{
							// 舍去 cond1 的情形。
							conditions.pop_back();
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == GTE || cond2.op == GT))
						{
							// 舍去 cond2 的情形。
							conditions.erase(conditions.begin() + i);
							i--;
						}
						else if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == GTE || cond2.op == GT))
						{
							// 取区间的情形。
							conditions.erase(conditions.begin() + i);
							i--;
							conditions.pop_back();
							if (cond1.type == 'd')
							{
								Condition cond3 = Condition(cond1.attr, cond2.value1.d, cond2.op == GTE, cond1.value1.d, cond1.op == LTE);
								conditions.push_back(cond3);
							}
							else if (cond1.type == 'f')
							{
								Condition cond3 = Condition(cond1.attr, cond2.value1.f, cond2.op == GTE, cond1.value1.f, cond1.op == LTE);
								conditions.push_back(cond3);
							}
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == LTE || cond2.op == LT))
						{
							// 条件不可能成立。
							throw DoNotMatch();
						}
					}
				}
			}
		}
	}
}

// fieldsProcess 函数：处理字段列表。
// 输入：
// table 为数据表的名称。
// info 为字段的表达式。
// 输出：
// fieldList 为字段的列表。
void fieldsProcess(const string table, const string info, vector<string> &fieldList)
{
	if ((info.substr(info.find_first_not_of(' '), info.find_last_not_of(' ') + 1)) == "*")
	{
		const vector<field> fields = getFields(currentDatabase, table);
		for (unsigned int i = 0; i < fields.size(); i++)
		{
			fieldList.push_back(fields[i].fieldName);
		}
	}
	else
	{
		int lpos = 0, rpos = 0; // lpos 和 rpos 用于将一大坨语句按照逗号分段，便于进一步处理。
		vector<string> fields;  // 各个要插入的值。
		do {
			string line; // 要处理的语句分段。
			rpos = info.find(',', lpos);
			if (rpos != string::npos)
			{
				line = info.substr(lpos, rpos - lpos);
			}
			else
			{
				line = info.substr(lpos, info.length() - lpos);
			}
			lpos = rpos + 1;
			line = line.substr(line.find_first_not_of(' '), line.find_last_not_of(' ') + 1);
			fields.push_back(line);
		} while (rpos != string::npos);
		// 判断字段是否存在。
		for (unsigned int i = 0; i < fields.size(); i++)
		{
			if (getField(currentDatabase, table, fields[i]).fieldName == "")
			{
				throw SqlError("field \"" + fields[i] + "\"doesn't exist");
			}
			fieldList.push_back(fields[i]);
		}
	}
}

// recordProcess 函数：根据条件一步步得到一个表中符合条件的偏移量列表。
// 输入：
// table 为数据表的名称。
// conditions 为给定的条件。
struct recordRange recordProcess(const string table, vector<Condition> &conditions)
{
	// 第一步：根据索引找出包含的记录和不包含的记录。
	vector<int> range;		// 记录的最大范围。
	vector<int> excluded;	// 需要排除的记录。
	bool norange = true;	// 是否目前没有记录范围（和range为空进行区分）
	bool noexcluded = true;	// 是否目前没有排除记录（和excluded为空进行区分）
	for (vector<Condition>::iterator it = conditions.begin(); it != conditions.end(); it++)
	{
		vector<int> range2, excluded2;
		bool range2used = false, excluded2used = false;
		string index = "";	// 要查找数据的索引的全称。
		string indexName = indexInField(currentDatabase, table, (*it).attr);
		if (indexName != "")
		{
			index = currentDatabase + "-" + indexName;
		}
		else if (isPrimaryKey(currentDatabase, table, (*it).attr))
		{
			index = currentDatabase + "#" + table;
		}
		if (index != "")
		{
			if ((*it).op == EQ)
			{
				if ((*it).type == 'c')
				{
					range2.push_back(findIndexNode(index, (*it).value1.c));
					range2used = true;
				}
				else if ((*it).type == 'd')
				{
					range2.push_back(findIndexNode(index, (*it).value1.d));
					range2used = true;
				}
				else if ((*it).type == 'f')
				{
					range2.push_back(findIndexNode(index, (*it).value1.f));
					range2used = true;
				}
			}
			else if ((*it).op == DIFF)
			{
				if ((*it).type == 'c')
				{
					excluded2.push_back(findIndexNode(index, (*it).value1.c));
					excluded2used = true;
				}
				else if ((*it).type == 'd')
				{
					excluded2.push_back(findIndexNode(index, (*it).value1.d));
					excluded2used = true;
				}
				else if ((*it).type == 'f')
				{
					excluded2.push_back(findIndexNode(index, (*it).value1.f));
					excluded2used = true;
				}
			}
			else if ((*it).op == GT || (*it).op == GTE)
			{
				if ((*it).type == 'd')
				{
					range2 = findIndexNodes(index, (*it).value1.d, (*it).op == GTE);
					range2used = true;
				}
				else if ((*it).type == 'f')
				{
					range2 = findIndexNodes(index, (*it).value1.f, (*it).op == GTE);
					range2used = true;
				}
			}
			else if ((*it).op == LT || (*it).op == LTE)
			{
				if ((*it).type == 'd')
				{
					excluded2 = findIndexNodes(index, (*it).value1.d, (*it).op == LT);
					excluded2used = true;
				}
				else if ((*it).type == 'f')
				{
					excluded2 = findIndexNodes(index, (*it).value1.f, (*it).op == LT);
					excluded2used = true;
				}
			}
			else if ((*it).op == BETW)
			{
				if ((*it).type == 'd')
				{
					range2 = findIndexNodes(index, static_cast<const float>((*it).value1.d), (*it).eq1, static_cast<const float>((*it).value2.d), (*it).eq2);
					range2used = true;
				}
				else if ((*it).type == 'f')
				{
					range2 = findIndexNodes(index, (*it).value1.f, (*it).eq1, (*it).value2.f, (*it).eq2);
					range2used = true;
				}
			}

			if (range2used)
			{
				if (norange)
				{
					range = range2;
					norange = false;
				}
				else
				{
					range = unionOffsets(range, range2);
				}
			}
			if (excluded2used)
			{
				if (noexcluded)
				{
					excluded = excluded2;
					noexcluded = false;
				}
				else
				{
					excluded = mergeOffsets(excluded, excluded2);
				}
			}

			conditions.erase(it);
		}
	}
	struct recordRange records = { range, excluded, norange };
	return records;
}

// mergeOffsets 函数：合并两个 offset 的 vector。
vector<int> mergeOffsets(vector<int> v1, vector<int> v2)
{
	vector<int>::iterator i1 = v1.begin(), i2 = v2.begin();
	vector<int> v3;
	while (i1 != v1.end() && i2 != v2.end())
	{
		if ((*i1) == (*i2))
		{
			i1++;
		}
		else if ((*i1) < (*i2))
		{
			v3.push_back(*i1);
			i1++;
		}
		else
		{
			v3.push_back(*i2);
			i2++;
		}
	}
	while (i1 != v1.end())
	{
		v3.push_back(*i1);
		i1++;
	}
	while (i2 != v2.end())
	{
		v3.push_back(*i2);
		i2++;
	}
	return v3;
}

// unionOffsets 函数：取 v1 和 v2 的交集。
vector<int> unionOffsets(vector<int> v1, vector<int> v2)
{
	vector<int>::iterator i1 = v1.begin(), i2 = v2.begin();
	vector<int> v3;
	while (i1 != v1.end() && i2 != v2.end())
	{
		if ((*i1) > (*i2))
		{
			i2++;
		}
		if ((*i1) < (*i2))
		{
			i1++;
		}
		if ((*i1) == (*i2))
		{
			v3.push_back(*i1);
			i2++;
		}
	}
	return v3;
}

// excludeOffsets 函数：在 v1 中排除 v2 元素。
vector<int> excludeOffsets(vector<int> v1, vector<int> v2)
{
	int i1 = 0, i2 = 0;
	while (i1 < v1.size() && i2 < v2.size())
	{
		if (v1[i1] > v2[i2])
		{
			i2++;
		}
		else if (v1[i1] < v2[i2])
		{
			i1++;
		}
		else if (v1[i1] == v2[i2])
		{
			v1.erase(v1.begin() + i1);
			i2++;
		}
	}
	return v1;
}