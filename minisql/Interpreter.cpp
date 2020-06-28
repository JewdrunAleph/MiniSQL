#include"interpreter.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
command Interpreter(string statement)
{
	string SQL;
	string temp;
	string sql;
	command cmd;
	//初始化cmd
	strcpy_s(cmd.op, "");
	int start = 0, end;
	if (statement.empty())
		//获取用户输入
		SQL = read_input();
	else
		SQL = statement;
	//获取输入的第一个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start); //记录第一个单词
	start = end + 1;
	//若无输入，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//若为create语句
	else if (temp == "create")
		cmd = create_clause(SQL, start, cmd);
	//若为drop语句
	else if (temp == "drop")
		cmd = drop_clause(SQL, start,cmd);
	//若为select语句
	else if (temp == "select")
		cmd = select_clause(SQL, start,cmd);
	//若为insert语句
	else if (temp == "insert")
		cmd = insert_clause(SQL, start,cmd);
	//若为delete语句
	else if (temp == "delete")
		cmd = delete_clause(SQL, start,cmd);
	//若为use语句
	else if (temp == "use")
		cmd = use_clause(SQL, start,cmd);
	//若为execfile语句
	else if (temp == "execfile")
		cmd = execfile_clause(SQL, start,cmd);
	//若为quit语句
	else if (temp == "quit")
		cmd = quit_clause(SQL, start,cmd);
	//获取帮助
	else if (temp == "help")
		strcpy_s(cmd.op,"60");
	//若为非法语句
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//返回输入语句的内部形式
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//读取用户输入
string read_input()
{
	string SQL;
	string temp;
	char str[100];
	bool finish = false;
	while (!finish)
	{
		cin >> str;
		temp = str;
		SQL = SQL + " " + temp;
		if (SQL.at(SQL.length() - 1) == ';')
		{
			SQL[SQL.length() - 1]= ' ';
			SQL += ";";
			finish = true;
		}
	}
	//将输入大写转化为小写
	transform(SQL.begin(), SQL.end(), SQL.begin(), ::tolower);
	//返回用户输入
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
command create_database(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: database name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//返回create database语句的内部形式
		else
		{
			strcpy_s(cmd.op, "00");
			cmd.arg1 = temp;
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
command create_table(string SQL, int start,command cmd)
{
	string temp, sql;
	int end, length;
	//获取表名
	while (SQL.at(start) == ' ')
		start++;
	if ((end = SQL.find('(', start)) == -1)
	{
		cout << "syntax error: missing ( in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}
	temp = SQL.substr(start, end - start);

	//若表名非空
	if (!temp.empty())
	{
		//处理表名(删去空格)
		length = temp.length() - 1;
		while (temp.at(length) == ' ')
			length--;
		temp = temp.substr(0, length + 1);
		cmd.arg1 = temp;
	}

	//获取arg2
	start = end + 1;
	while (SQL.at(start) == ' ')
		start++;
	if ( (end = SQL.find_last_of(')')) == -1)
	{
		cout << "syntax error: missing ) in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}
	
	temp = SQL.substr(start, end - start);
	strcpy_s(cmd.op, "10");
	cmd.arg2 = temp;
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index on语句是否有效
command create_index_on(string SQL, int start, string sql,command cmd)
{
	string temp;
	int end, length;
	//获取表名
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find('(', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		cout << "\t missing ( !" << endl;
		strcpy_s(cmd.op, "99");
		//return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length = temp.length() - 1;
		while (temp.at(length) == ' ')
			length--;
		temp = temp.substr(0,length + 1);
		//有效
		if (temp.find(' ') == -1)
		{
			sql += " " + temp;
			cmd.arg2=temp;
			//获取属性名
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(')', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for create index statement!" << endl;
				cout << "\t missing ) !" << endl;
				strcpy_s(cmd.op, "99");
				//return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length = temp.length() - 1;
				while (temp.at(length) == ' ')
					length--;
				temp = temp.substr(0,length + 1);
				//有效
				if (temp.find(' ') == -1)
				{
					sql += " " + temp;
					cmd.arg3 = temp;
					while (SQL.at(start) == ' ')
						start++;
					if (SQL.at(start) != ';' || start != SQL.length() - 1)
					{
						cout << "syntax error: syntax error for quit!" << endl;
						strcpy_s(cmd.op, "99");
						//return SQL;
					}
					//返回create index语句的内部形式
					else {
						strcpy_s(cmd.op, "20");
					}
						
				}
				//无效,打印出错信息
				else
				{
					cout << "error:" << " " << temp << "---is not a valid attribute name!" << endl;
					strcpy_s(cmd.op, "99");
					//return SQL;
				}
			}
		}
		//无效,打印出错信息
		else
		{
			cout << "error:" << " " << temp << "---is not a valid table name!" << endl;
			strcpy_s(cmd.op, "99");
			//return SQL;
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index语句是否有效
command create_index(string SQL, int start,command cmd)
{
	string temp, sql;
	int end;
	//获取第三个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		strcpy_s(cmd.op,"99");
	}
	else
	{
		sql = temp;
		cmd.arg1 = temp;
		//获取第四个单词
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for create index statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//若为on,继续验证
		else if (temp == "on")
			cmd = create_index_on(SQL, start, sql,cmd);
		//若为非法信息，打印非法信息
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			strcpy_s(cmd.op, "99");
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
command create_clause(string SQL, int start,command cmd)
{
	string temp;
	int end;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//若为database,继续验证
	else if (temp == "database")
		cmd = create_database(SQL, start,cmd);
	//若为table,继续验证
	else if (temp == "table")
		cmd = create_table(SQL, start,cmd);
	//若为index,继续验证
	else if (temp == "index")
		cmd = create_index(SQL, start,cmd);
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//返回create语句的内部形式
	return cmd;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
command drop_database(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: database name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//返回drop database语句的内部形式
		else {
			strcpy_s(cmd.op, "02");
			cmd.arg1 = temp;
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
command drop_table(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: table name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid table name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//返回drop table语句的内部形式
		else {
			strcpy_s(cmd.op, "11");
			cmd.arg1 = temp;
		}
	}

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
command drop_index(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: index name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid index name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//返回drop index语句的内部形式
		else {
			strcpy_s(cmd.op, "21");
			cmd.arg1 = temp;
		}
	}

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
command drop_clause(string SQL, int start,command cmd) {
	string temp;
	int end;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for drop statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//若为database,继续验证
	else if (temp == "database")
		cmd = drop_database(SQL, start,cmd);
	//若为table,继续验证
	else if (temp == "table")
		cmd = drop_table(SQL, start,cmd);
	//若为index,继续验证
	else if (temp == "index")
		cmd = drop_index(SQL, start,cmd);
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//返回drop语句的内部形式
	return cmd;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证select 语句是否有效
command select_clause(string SQL, int start,command cmd) {
	string temp, sql, table;
	int end;
	//获取列名
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		cmd.arg1 = temp;
		//获取第三个单词
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for select statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		else if (temp == "from"){
			//获取第四个单词
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: missing ; in the statement!" << endl;
				strcpy_s(cmd.op, "99");
			}
			else {
				cmd.arg2 = temp;
				//获取第五个单词（是否有where）
				while (SQL.at(start) == ' ')
					start++;
				end = SQL.find(' ', start);
				temp = SQL.substr(start, end - start);
				start = end + 1;
				//若无，打印出错信息
				if (temp.empty())
				{
					cout << "syntax error: missing ; in the statement!" << endl;
					strcpy_s(cmd.op, "99");
				}
				else if (temp == ";") {
					strcpy_s(cmd.op, "31");
				}
				else if (temp == "where") {
					strcpy_s(cmd.op, "32");
					//获取condition信息
					end = SQL.find(';');
					while (SQL.at(start) == ' ') start++;
					temp = SQL.substr(start, end - start - 1);
					if (temp.empty()) {
						cout << "syntax error: missing condition in the statement!" << endl;
						strcpy_s(cmd.op, "99");
					}
					else cmd.arg3 = temp;
				}
				//若为非法信息，打印非法信息
				else
				{
					cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
					strcpy_s(cmd.op, "99");
				}
			}
			
		}
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			strcpy_s(cmd.op, "99");
		}
	}

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//验证insert 语句是否有效
command insert_clause(string SQL, int start,command cmd) {
	string temp, sql;
	int end;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for insert statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//若为into,继续验证
	else if (temp == "into") {
		//获取第三个单词(表名)
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for insert statement!" << endl;
			strcpy_s(cmd.op, "99");
			return cmd;
		}
		else {
			sql ="30"+ temp;
			cmd.arg1 = temp;
			//获取第四个单词
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find('(', start);
			temp = SQL.substr(start, end - start);
			start = end;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert statement!" << endl;
				strcpy_s(cmd.op, "99");
				return cmd;
			}
			else if (temp == "values") {
				cmd = insert_into_values(SQL, start, sql,cmd);
			}
			//若为错误信息，打印出错信息
			else
			{
				cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
				strcpy_s(cmd.op, "99");
				return cmd;
			}
		}
	}
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//返回drop语句的内部形式
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//验证insert into values语句是否有效
command insert_into_values(string SQL, int start, string sql,command cmd) {
	string temp;
	int end, length;

	if ((start = SQL.find('(', start)) == -1)
	{
		cout << "syntax error: missing ( in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}

	//获取arg2
	start++;
	while (SQL.at(start) == ' ')
		start++;
	if ((end = SQL.find_last_of(')')) == -1)
	{
		cout << "syntax error: missing ) in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}

	temp = SQL.substr(start, end - start);
	strcpy_s(cmd.op, "30");
	cmd.arg2 = temp;

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//验证delete语句是否有效
command delete_clause(string SQL, int start,command cmd) {
	string temp, sql,table;
	int end;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for delete statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else if (temp=="from")
	{
		//获取第三个单词（表名）
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for delete statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		else {
			table = temp;
			//获取第四个单词
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: missing ; in the statement!" << endl;
				strcpy_s(cmd.op, "99");
			}
			else if (temp == ";") {
				strcpy_s(cmd.op, "31");
				cmd.arg1 = table;
			}
			else if (temp == "where") {
				strcpy_s(cmd.op, "32");
				cmd.arg1 = table;
				//获取condition信息
				end = SQL.find(';');
				while (SQL.at(start) == ' ') start++;
				temp = SQL.substr(start, end - start - 1);
				if (temp.empty()) {
					cout << "syntax error: missing condition in the statement!" << endl;
					strcpy_s(cmd.op, "99");
				}
				else cmd.arg2 = temp;
			}
			//若为非法信息，打印非法信息
			else
			{
				cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
				strcpy_s(cmd.op, "99");
			}
		}
	}
	//若为非法信息，打印非法信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//验证use语句是否有效
command use_clause(string SQL, int start,command cmd) {
	string temp;
	int end,index;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for use statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//继续检查
	else if (SQL.at(start) != ';' || start != SQL.length() - 1)
	{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			strcpy_s(cmd.op, "99");
	}
	else {
		strcpy_s(cmd.op, "01");
		cmd.arg1 = temp;
	}

	return cmd;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//验证execfile语句是否有效
command execfile_clause(string SQL, int start,command cmd) {
	string temp;
	int end, index;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for execfile statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//继续检查
	else if (SQL.at(start) != ';' || start != SQL.length() - 1)
	{
		cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid txt name!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else {
		strcpy_s(cmd.op, "40");
		cmd.arg1 = temp;
	}

	return cmd;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//验证quit语句是否有效
command quit_clause(string SQL, int start,command cmd) {
	string temp;
	int end;
	//获取第二个单词
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for quit statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//若为;,退出程序
	else if (temp == ";") {
		cout << "exiting program..." << endl;
		strcpy_s(cmd.op, "50");
	}
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}

	return cmd;
}