#include "API.h"

using namespace std;

/********************
* ע�⣺Ŀǰ��δ���û�о������ԣ������ο�������֤������ȷ�ԡ�
* ���ң�Ŀǰֻ����� API ģ��ĺ��ĺ����������������ģ��Խ���Ҫ�������������
* API ģ��ı�д˼·�ο� miniSQL ��ٺ͸��� PPT��˼·���������ĵ�Ϊ�������������̡�
*/

// TODO: ��ѯ���Ŀǰû������ index��֮��Ҫ��һ�¡�

static string currentDatabase = "";    // ��ǰʹ�õ����ݿ����ơ�

// executeCommand ������API ģ��ĺ��ĺ�����
// ���������� interpreter ����õ�������ָ� Catalog Manager, Record Manager �� Index Manager ���д���
// ������������ cmd Ϊ struct command������Ľ��ͼ�"interpreter.h"�ļ���
void executeCommand(const struct command cmd)
{
	// ���ݿ���ز�����Interpreter ֻ��Ҫ��֤�﷨��ȷ������Ҫ��֤���ݿ�Ĵ������
	if (cmd.op == "00")
	{
		// �������ݿ⣬cmd �� op Ϊ"00"��
		// arg1 Ϊ���ݿ�����֣�interpreter ��Ҫ��֤�䲻��Ϊ���ҷ��������淶�������ԱΪ�ա�
		// (������˵��ʵ���ǵĴ�̴����ûҪ�����ݿ⣬���Ǵ�̸���ppt�������еģ������д����˵)
		if (databaseExists(cmd.arg1))
		{
			throw SqlError("Database already exists.");
		}
		createDatabase(cmd.arg1);
	}
	else if (cmd.op == "01")
	{
		// ʹ��ָ�������ݿ⣬cmd �� op Ϊ"01"��
		// arg1 Ϊ���ݿ�����֣�interpreter ��Ҫ��֤�䲻��Ϊ�գ������ԱΪ�ա�
		if (!databaseExists(cmd.arg1))
		{
			throw SqlError("Database does not exist.");
		}
		currentDatabase = cmd.arg1;
	}
	else if (cmd.op == "02")
	{
		// ɾ�����ݿ⣬cmd �� op Ϊ"02"��
		// arg1 Ϊ���ݿ�����֣�interpreter ��Ҫ��֤�䲻��Ϊ�գ������ԱΪ�ա�
		if (!databaseExists(cmd.arg1))
		{
			throw SqlError("Database does not exist.");
		}
		vector<string> tableList = tablesInDatabase(cmd.arg1);
		// ɾ�����ݱ����ݱ��е�������
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
		// ���ݱ���ز�����Interpreter ֻ��Ҫ��֤�﷨��ȷ������Ҫ��֤���ݱ�Ĵ������
		if (cmd.op == "10")
		{
			// �������ݱ�cmd �� op Ϊ"10"��
			// arg1 Ϊ���ݱ�����֣�������������淶����arg2 Ϊ�����"(...)"�ڵ�����(���������ű������ǲ����л��У�)��arg2������Ҫ����֤�﷨��ȷ��api ģ�����м��ͷ�����
			if (tableExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Table already exists.");
			}
			vector<struct field> fields; // �������ɵĸ����ֶΣ�����Ľ��ͼ�"catalog.h"�ļ���
			string primaryKey = ""; // ���ݱ�����롣
			tableCreateProcess(cmd.arg2, fields, primaryKey); // ���� arg2. ��Ϊʵ������̫������˵�����Ϊһ��������
			createTable(currentDatabase, cmd.arg1, fields, primaryKey);
			initTable(currentDatabase + "-" + cmd.arg1, fields);
			// �������Զ�����һ��������Ϊ�˷�ֹ����������������Ϊ"<���ݿ���>#<���ݱ���>"��
			// �����봴������������ catalog manager ��������
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
			// ɾ�����ݱ�cmd �� op Ϊ"11"��
			// arg1 Ϊ���ݱ�����֣�interpreter ��Ҫ��֤�䲻��Ϊ�ա���ģ�鲻��֤���ݱ��ܷ�ɾ����ֻ�ܱ�֤��ɾ�������ݱ���ڡ�
			if (!tableExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Table does not exist.");
			}
			// ɾ�����ݱ����ݱ��е�������
			vector<string> indice = indiceInTable(currentDatabase, cmd.arg1);
			for (unsigned int i = 0; i < indice.size(); i++)
			{
				deleteIndex(currentDatabase + "-" + indice[i]);
			}
			// ɾ�����ݱ������������
			const string primaryKey = getPrimaryKey(currentDatabase, cmd.arg1);
			if (primaryKey != "")
			{
				deleteIndex(currentDatabase + "#" + cmd.arg1);
			}
			deleteTable(currentDatabase + "-" + cmd.arg1);
			dropTable(currentDatabase, cmd.arg1);
		}
		// ������ز�����Interpreter ֻ��Ҫ��֤�﷨��ȷ������Ҫ��֤�����Ĵ������
		else if (cmd.op == "20")
		{
			// ����������cmd �� op Ϊ"20"��
			// arg1 Ϊ���������֣�arg2 Ϊ������arg3 Ϊ�ֶ�����
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
			const struct field fieldInfo = getField(currentDatabase, cmd.arg2, cmd.arg3); // Ҫ���������ֶεľ�����Ϣ��
			if (!fieldInfo.unique) // miniSQL �е�Ҫ��Ϊ�����ǵ�ֵ������
			{
				throw SqlError("Index should be created on unique field for miniSQL.");
			}
			else
			{
				createIndex(currentDatabase, cmd.arg2, cmd.arg3, cmd.arg1);
				// ���������������ʼֵ��
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
			// ɾ��������cmd �� op Ϊ"21"��
			// arg1 Ϊ���������֡�
			if (!indexExists(currentDatabase, cmd.arg1))
			{
				throw SqlError("Index doesn't exist.");
			}
			dropIndex(currentDatabase, cmd.arg1);
			deleteIndex(currentDatabase + "-" + cmd.arg1);
		}
		else if (cmd.op == "30")
		{
			// �����¼��cmd �� op Ϊ"30"��һ��ֻ�ܲ���һ����¼��
			// arg1 Ϊ���ݱ�����֡�
			// arg2 Ϊ�����"(...)"�ڵ�����(���������ű������ǲ����л��У�)
			// ��Ϊ miniSQL �������ֵ����˲���ֻ�ܲ���ȫ���ֶΡ�
			if (!tableExists(currentDatabase, cmd.arg1)) //������ݱ����� 
			{
				throw SqlError("Table doesn't exist.");
			}
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // Ҫ��������ݱ���ֶ���Ϣ��
			vector<Record_node> record; // Ҫ����ļ�¼����Ϣ��
			recordInsertProcess(cmd.arg1, cmd.arg2, fields, record);	// ���� arg2. ��Ϊʵ������̫������˵�����Ϊһ��������
			// �������ݡ�
			const int offset = insertRecord(currentDatabase + "-" + cmd.arg1, record);
			// �ж����ݱ��Ƿ��������������������Ӧ���ݡ�
			vector<string> indice;
			for (unsigned int i = 0; i < fields.size(); i++)
			{
				string index = "";	// Ҫ�������ݵ�������ȫ�ơ�
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
			// ɾ����¼(�� where �Ӿ�)��cmd �� op Ϊ"31"��
			// arg1 Ϊ���ݱ�����֡�
			if (!tableExists(currentDatabase, cmd.arg1)) //������ݱ����� 
			{
				throw SqlError("Table doesn't exist.");
			}
			// �жϸ����ݱ��Ƿ����������������������ǡ�
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
			// ������ݱ�
			clearRecord(currentDatabase + "-" + cmd.arg1);
		}
		else if (cmd.op == "32")
		{
			// ɾ����¼(�� where �Ӿ�)��cmd �� op Ϊ"32"��
			// arg1 Ϊ���ݱ�����֡�
			// arg2 Ϊ where �Ӿ��е�����(����"where"�ؼ��ʺͷֺ�)��
			if (!tableExists(currentDatabase, cmd.arg1)) //������ݱ����� 
			{
				throw SqlError("Table doesn't exist.");
			}
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // Ҫ��������ݱ���ֶ���Ϣ��
			vector<Condition> conditions;
			try		// �����϶�����ƥ������Ρ�
			{
				conditionProcess(cmd.arg2, fields, conditions);
				struct recordRange recordRange = recordProcess(cmd.arg1, conditions);
				vector<struct Record> results;
				// range ���ų� excluded��
				if (!recordRange.norange)
				{
					recordRange.range = excludeOffsets(recordRange.range, recordRange.excluded);
					results = findRecords(currentDatabase + "-" + cmd.arg1, conditions, recordRange.range, recordRange.excluded);
				}
				else
				{
					results = findRecords(currentDatabase + "-" + cmd.arg1, conditions, recordRange.excluded);
				}
				// ѡ������ index ���ֶβ���¼���ֶ�����
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
			// ѡ���¼���� where �Ӿ䣩��cmd �� op Ϊ"33"��
			// arg1 Ϊѡ����ֶ�("*"����ȫ��)��
			// arg2 Ϊѡ������ݱ�
			if (!tableExists(currentDatabase, cmd.arg2)) //������ݱ����� 
			{
				throw SqlError("Table doesn't exist.");
			}
			vector<string> fieldList;
			fieldsProcess(cmd.arg2, cmd.arg1, fieldList);
			printRecords(currentDatabase + "-" + cmd.arg2, fieldList);
		}
		else if (cmd.op == "34")
		{
			// ѡ���¼���� where �Ӿ䣩��cmd �� op Ϊ"33"��
			// arg1 Ϊѡ����ֶ�("*"����ȫ��)��
			// arg2 Ϊѡ������ݱ�
			// arg3 Ϊ where �Ӿ��е�����(����"where"�ؼ��ʺͷֺ�)��
			if (!tableExists(currentDatabase, cmd.arg2)) //������ݱ����� 
			{
				throw SqlError("Table doesn't exist.");
			}
			vector<string> fieldList;
			fieldsProcess(cmd.arg2, cmd.arg1, fieldList);
			const vector<struct field> fields = getFields(currentDatabase, cmd.arg1); // Ҫ��������ݱ���ֶ���Ϣ��
			vector<Condition> conditions;
			try		// �����϶�����ƥ������Ρ�
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

// inCondInterval �������ж�һ����ֵ�Ƿ��������������ڡ�
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

// ���µ������� API ģ�鴦��һЩ�е�û�����⣬�������ã���������Щ������Ȥ���Բ��ÿ���ȥ�ˡ�
// ֮����ܻ����Щ����������һ���ļ���ߡ�

// tableCreateProcess ���������������ݱ�ʱ�Ĳ�����
// ���룺
// info Ϊ�������ݱ�����ж����ݱ����ݶ�����ַ�����
// �����
// fields Ϊ���ɵ����ݱ���ֶ���Ϣ��
// primaryKey Ϊ���ɵ����ݱ�����롣
void tableCreateProcess(const string info, vector<struct field>& fields, string &primaryKey)
{
	int lpos = 0, rpos = 0; // lpos �� rpos ���ڽ�һ������䰴�ն��ŷֶΣ����ڽ�һ������
	string line; // Ҫ��������ֶΡ�
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

		int state = 0; // �жϵ�ǰ�����״̬��
		int wpos = 0; // ��ǰ���ʵ���硣
		vector<string> words;
		unsigned int i;
		// ��ʼ�����������⡣
		for (i = 0; i < line.length(); i++)
		{
			if (state == 0) // ��ȡ�ո�
			{
				if (line[i] == '(') // ��ȡ��Ϊ�����������
				{
					wpos = i + 1;
					state = 2;
				}
				else if (line[i] != ' ') // ��ʼ�����ʡ�
				{
					state = 1;
					wpos = i;
				}
			}
			else if (state == 1) // ��ȡ���ʡ�
			{
				if (line[i] == '(') // ��ȡ��Ϊ�����������
				{
					words.push_back(line.substr(wpos, i - wpos));
					wpos = i + 1;
					state = 2;
				}
				else if (line[i] == ' ') // ���ʶ�ȡ��ϣ���ʱ��ʼ�����ʡ�
				{
					words.push_back(line.substr(wpos, i - wpos));
					state = 0;
				}
			}
			else if (state == 2) // ��ȡ������
			{
				if (line[i] == ')') // ������ȡ��ϡ�
				{
					string param;
					param = line.substr(wpos, i - wpos);
					size_t testa = param.find_first_not_of(' ');
					size_t testb = param.find_last_not_of(' ');
					param = param.substr(param.find_first_not_of(' '), param.find_last_not_of(' ') + 1);
					param = "(" + param + ")"; // ���������и�ʽ����
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
		// �ж����塣
		if (words.size() >= 3 && words[0] == "primary" && words[1] == "key" \
			&& words[2][0] == '(' && words[2][words[2].length() - 1] == ')')
		{
			// ��������롣
			if (primaryKey != "")  // ������ĿҪ��miniSQL ��֧�ֶ�����������
			{
				throw SqlError("Multiple primary keys. This feature is not supported in miniSQL.");
			}
			for (unsigned int i = 0; i < fields.size(); i++)  // SQL Ҫ��������ֶα�����ǰ���Ѿ����塣
			{
				if (fields[i].fieldName == words[2].substr(1, words[2].length() - 2))
				{
					primaryKey = fields[i].fieldName;
					fields[i].unique = true;	// ����ǿ�� unique��
				}
			}
			if (primaryKey == "")  // ������Ч�������
			{
				throw SqlError("Field undefined for primary key definition.");
			}
		}
		else if (words.size() < 2)
		{
			// ���û�б������͡�
			throw SqlError("Undefined field type.");
		}
		else
		{
			string fieldName, fieldType; // �����õ����ֶ���Ϣ��
			unsigned int fieldSize = 0; // �����õ����ֶγ��ȡ�
			bool unique = false; // �����õ����ֶ��Ƿ�Ψһ��
			bool flag = false; // �ж��ֶ����Ƿ���Ч��SQL ��Ҫ���ֶ���������Ϊ��ĸ�����ֻ��»��ߣ������ַ�����Ϊ���ֵ�Ҫ��
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
				if (words.size() > 4 || words.size() < 3 || (words.size() == 4 && words[3] != "unique")) // �����ʽ����ȷ��
				{
					throw SqlError("Invalid syntax.");
				}
				bool flag = false; // �ж����ֲ������Ƿ��з������ַ���
				for (unsigned int i = 1; words[2][i] != ')' && i < words[2].length(); i++)
				{
					if (words[2][i] < '0' || words[2][1] > '9')
					{
						flag = true;
					}
				}
				if (flag) // ����з������ַ���
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should be numeric.");
				}
				try
				{
					fieldSize = stoi(words[2].substr(1, words[2].length() - 2));
				}
				catch (std::out_of_range) // �������Խ��Ĵ���
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should between 1 and 255.");
				}
				if (fieldSize <= 0 || fieldSize > 255) // miniSQL Ҫ���ַ����ֶεĳ����ڲ�����255.
				{
					throw SqlError("Char length of field \"" + words[0] + "\" should between 1 and 255.");
				}
				if (words.size() == 4 && words[3] == "unique")
				{
					unique = true;
				}
				if (fields.size() >= 32) // ����ֶ�̫��
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
				if (words.size() > 3 || words.size() < 2 || (words.size() == 3 && words[2] != "unique")) // �����ʽ����ȷ��
				{
					throw SqlError("Invalid syntax.");
				}
				if (fields.size() >= 32) // ����ֶ�̫��
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
			// �ж��ֶ��Ƿ�����
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
// recordInsertProcess ����������������ݵĺ�����
// ���룺
// table Ϊ���ݱ�����
// info Ϊ�������ݱ��ֵ����Ϣ��
// fields Ϊ������ı���ֶ���Ϣ��
// �����
// record ΪҪ����ļ�¼����Ϣ��
void recordInsertProcess(const string table, const string info, const vector<struct field> fields, vector<Record_node> &record)
{
	int lpos = 0, rpos = 0; // lpos �� rpos ���ڽ�һ������䰴�ն��ŷֶΣ����ڽ�һ������
	vector<string> values;  // ����Ҫ�����ֵ��
	do {
		string line; // Ҫ��������ֶΡ�
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
				if (values[i][j] <'0' || values[i][j] > '9') // ���Ϊ������
				{
					throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" does not match the type \"int\".");
				}
			}
			int value;
			try
			{
				value = stoi(values[i]);
			}
			catch (out_of_range) // �������Խ��Ĵ���
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" out of range.");
			}
			if (isPrimaryKey(currentDatabase, table, fields[i].fieldName))
			{
				// ����� unique ���ԡ�
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique ���ԡ�
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
				int flag;	// �Ƿ��Ѿ����ֹ�һ��С���㡣
				if (values[i][j] <'0' || values[i][j] > '9' || (values[i][j] == '.' && (flag || j == 0 || j == values[i].length() - 1))) // ���Ϊ�����ֻ�С����λ�ò���
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
			catch (out_of_range) // �������Խ��Ĵ���
			{
				throw SqlError("Value " + values[i] + " for field \"" + fields[i].fieldName + "\" out of range.");
			}
			if (isPrimaryKey(currentDatabase, table, fields[i].fieldName))
			{
				// ����� unique ���ԡ�
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique ���ԡ�
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
				// ����� unique ���ԡ�
				if (findIndexNode(currentDatabase + "#" + table, value) != -1)
				{
					throw SqlError("Duplicate value " + values[i] + " for unique field \"" + fields[i].fieldName + "\".");
				}
			}
			else if (fields[i].unique)
			{
				// unique ���ԡ�
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

// conditionProcess ���������� where �Ӿ��ڵ�������
// ���룺
// info Ϊ where �Ӿ��ڵ��������ʽ��
// fields Ϊ��ѯ�ı���ֶ���Ϣ��(�ڣ����ô�� miniSQL һ��ֻ����һ�ű�û�еѿ�������Ҫ��Ȼ�Ҿ����Һ�д record manager ���ϸ���Ե�������ȥ��...)
// �����
// conditions Ϊ������ϵ���������Ϣ��
void conditionProcess(const string info, const vector<struct field> fields, vector<Condition> &conditions)
{
	int lpos = 0, rpos = 0; // lpos �� rpos ���ڽ�һ������䰴�ն��ŷֶΣ����ڽ�һ������
	vector<string> lines;  // ����Ҫ�����ֵ��
	do {
		string line; // Ҫ��������ֶΡ�
		rpos = info.find("and", lpos);	// miniSQL �ؼ���Ҫ��ȫСд����������ֻ�� and...
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
		int oppos;	// ��������λ�á�
		condop op;	// �����������͡�
		string attr;		// Ҫ���������ԡ�
		string valueStr;	// Ҫ������ֵ���ַ�����ʽ��
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
		catch (out_of_range)	// attr ���� value ��ĳһ��Ϊ��
		{
			throw SqlError("Invalid syntax for where clause.");
		}
		int fieldId = -1;	// attr ��ָ����ֶε� index.
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
		// ��ʼ�ж������Ƿ���ȷ��
		if (fields[fieldId].fieldType == "int")
		{
			for (unsigned int j = 0; j < valueStr.length(); j++)
			{
				if (valueStr[j] <'0' || valueStr[j] > '9') // ���Ϊ������
				{
					throw SqlError("Value " + valueStr + " for field \"" + attr + "\" does not match the type \"int\".");
				}
			}
			int value;
			try
			{
				value = stoi(valueStr);
			}
			catch (out_of_range) // �������Խ��Ĵ���
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
				int flag;	// �Ƿ��Ѿ����ֹ�һ��С���㡣
				if (valueStr[j] <'0' || valueStr[j] > '9' || (valueStr[j] == '.' && (flag || j == 0 || j == valueStr.length() - 1))) // ���Ϊ�����ֻ�С����λ�ò���
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
			catch (out_of_range) // �������Խ��Ĵ���
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
		Condition cond1 = conditions[conditions.size() - 1];	// ��������������
		Condition cond2;										// Ҫ�Ƚϵ�������
		// �ж��Ƿ���Ժϲ�������
		bool flag = false;	// Ϊ true ��������Խ��������ϲ�
		int i;
		for (i = 0; i < conditions.size(); i++)	// ֮���Բ��õ���������Ϊ�ж��Ƿ�Ϊ���һ������鷳��
		{
			cond2 = conditions[i];
			if (cond2.attr == cond1.attr)
			{
				flag = true;
				break;	//����һ�����һ���ж�
			}
		}
		if (flag)
		{
			if (cond1.type == 'c')
			{
				// ���ַ������бȽ�
				if (cond1.op == EQ && cond2.op == EQ)
				{
					// ������Ϊ������������ʱֵһ����һ��ͬʱ���㣬��һ���򲻳�����
					if (cond1.value1.c == cond2.value1.c)
					{
						conditions.pop_back();	// ɾ��һ���ظ�������
					}
					else
					{
						throw DoNotMatch();
					}
				}
				else
				{
					// һ��Ϊ����һ��Ϊ�����ڣ���ʱֵһ���򲻳�������һ��ȡ���ڵ������
					if (cond1.value1.c == cond2.value1.c)
					{
						throw DoNotMatch();
					}
					else
					{
						if (cond1.op == EQ)
						{
							conditions.erase(conditions.begin() + i);	// ɾ�� cond2��
							i--;
						}
						else
						{
							conditions.pop_back();	// ɾ�� cond1��
						}
					}
				}
			}
			else
			{
				// ����ֵ���бȽϣ������Ϊ����һЩ��
				if (cond1.op == EQ)
				{
					// ��һ������Ϊ���ڣ���ôֻ�������������һ������ֵ��/���ڵڶ��������������ڡ�
					if (inCondInterval(cond1.getValue1(), cond2))
					{
						conditions.erase(conditions.begin() + i);		// ֵ�������ڣ����� cond2��
						i--;
					}
					else
					{
						throw DoNotMatch();			// ֵ���������ڣ�һ����������
					}
				}
				else if (cond2.op == EQ)
				{
					// ��һ������Ϊ���ڣ���ôֻ�������������һ������ֵ��/���ڵڶ��������������ڡ�
					if (inCondInterval(cond2.getValue1(), cond1))
					{
						conditions.pop_back();		// ֵ�������ڣ����� cond1��
					}
					else
					{
						throw DoNotMatch();			// ֵ���������ڣ�һ����������
					}
				}
				else if (cond1.op == DIFF && !inCondInterval(cond1.getValue1(), cond2))
				{
					// ��һ������Ϊ��������ֵ���ڵڶ��������������ڣ���������Ч������ cond1��
					conditions.pop_back();
				}
				else if (cond2.op == DIFF && !inCondInterval(cond2.getValue1(), cond1))
				{
					// ��һ������Ϊ��������ֵ���ڵڶ��������������ڣ���������Ч������ cond2��
					conditions.erase(conditions.begin() + i);
					i--;
				}
				else if (cond2.op == BETW)
				{
					// �ڶ�������Ϊ����ֵ֮�䣬��ô�Ե�һ��������ֵ�����жϡ���һ�������²��룬������Ϊ"BETW"����������Ǻϲ����������ɵģ���
					if ((cond1.getValue1() <= cond2.getValue1() && cond2.eq1 == false) \
						|| (cond1.getValue1() < cond2.getValue1() && cond2.eq1 == true))
					{
						// �����������������ֵС��
						if (cond1.op == LT || cond1.op == LTE)
						{
							// ��ʱ���������ܳ�����
							throw DoNotMatch();
						}
						else
						{
							conditions.pop_back();	// ��ʱ cond1 �������á�
						}
					}
					else if ((cond1.getValue1() >= cond2.getValue2() && cond2.eq2 == false) \
						|| (cond1.getValue1() > cond2.getValue2() && cond2.eq2 == true))
					{
						// �����������������ֵ��
						if (cond1.op == GT || cond1.op == GTE)
						{
							// ��ʱ���������ܳ�����
							throw DoNotMatch();
						}
						else
						{
							conditions.pop_back();	// ��ʱ cond1 �������á�
						}
					}
					else
					{
						// cond1 ��ֵ�� cond2 �����ڣ���ʱ�� cond2 �ķ�Χ����һЩ������
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
					// cond1 �� cond2 ��Ϊ����С�����������
					if (cond1.getValue1() < cond1.getValue1())
					{
						// cond1 ��ֵ�� cond2 С��
						if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == LTE || cond2.op == LT))
						{
							// ��ȥ cond2 �����Ρ�
							conditions.erase(conditions.begin() + i);
							i--;
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == GTE || cond2.op == GT))
						{
							// ��ȥ cond1 �����Ρ�
							conditions.pop_back();
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == LTE || cond2.op == LT))
						{
							// ȡ��������Ρ�
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
							// ���������ܳ�����
							throw DoNotMatch();
						}
					}
					else if (cond1.getValue1() == cond2.getValue1())
					{
						// ����ֵ��ȡ�
						if ((cond1.op == LTE || cond1.op == GTE) && (cond2.op == LTE || cond2.op == GTE))
						{
							// �������Ժϲ�����ȡ�
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
							// ���������ܳ�����
							throw DoNotMatch();
						}
					}
					else if (cond1.getValue1() > cond2.getValue1())
					{
						// cond1 ��ֵ�� cond2 ��
						if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == LTE || cond2.op == LT))
						{
							// ��ȥ cond1 �����Ρ�
							conditions.pop_back();
						}
						else if ((cond1.op == GTE || cond1.op == GT) && (cond2.op == GTE || cond2.op == GT))
						{
							// ��ȥ cond2 �����Ρ�
							conditions.erase(conditions.begin() + i);
							i--;
						}
						else if ((cond1.op == LTE || cond1.op == LT) && (cond2.op == GTE || cond2.op == GT))
						{
							// ȡ��������Ρ�
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
							// ���������ܳ�����
							throw DoNotMatch();
						}
					}
				}
			}
		}
	}
}

// fieldsProcess �����������ֶ��б�
// ���룺
// table Ϊ���ݱ�����ơ�
// info Ϊ�ֶεı��ʽ��
// �����
// fieldList Ϊ�ֶε��б�
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
		int lpos = 0, rpos = 0; // lpos �� rpos ���ڽ�һ������䰴�ն��ŷֶΣ����ڽ�һ������
		vector<string> fields;  // ����Ҫ�����ֵ��
		do {
			string line; // Ҫ��������ֶΡ�
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
		// �ж��ֶ��Ƿ���ڡ�
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

// recordProcess ��������������һ�����õ�һ�����з���������ƫ�����б�
// ���룺
// table Ϊ���ݱ�����ơ�
// conditions Ϊ������������
struct recordRange recordProcess(const string table, vector<Condition> &conditions)
{
	// ��һ�������������ҳ������ļ�¼�Ͳ������ļ�¼��
	vector<int> range;		// ��¼�����Χ��
	vector<int> excluded;	// ��Ҫ�ų��ļ�¼��
	bool norange = true;	// �Ƿ�Ŀǰû�м�¼��Χ����rangeΪ�ս������֣�
	bool noexcluded = true;	// �Ƿ�Ŀǰû���ų���¼����excludedΪ�ս������֣�
	for (vector<Condition>::iterator it = conditions.begin(); it != conditions.end(); it++)
	{
		vector<int> range2, excluded2;
		bool range2used = false, excluded2used = false;
		string index = "";	// Ҫ�������ݵ�������ȫ�ơ�
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

// mergeOffsets �������ϲ����� offset �� vector��
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

// unionOffsets ������ȡ v1 �� v2 �Ľ�����
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

// excludeOffsets �������� v1 ���ų� v2 Ԫ�ء�
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