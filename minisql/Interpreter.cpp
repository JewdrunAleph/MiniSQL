#include"interpreter.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
command Interpreter(string statement)
{
	string SQL;
	string temp;
	string sql;
	command cmd;
	//��ʼ��cmd
	strcpy_s(cmd.op, "");
	int start = 0, end;
	if (statement.empty())
		//��ȡ�û�����
		SQL = read_input();
	else
		SQL = statement;
	//��ȡ����ĵ�һ������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start); //��¼��һ������
	start = end + 1;
	//�������룬��ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//��Ϊcreate���
	else if (temp == "create")
		cmd = create_clause(SQL, start, cmd);
	//��Ϊdrop���
	else if (temp == "drop")
		cmd = drop_clause(SQL, start,cmd);
	//��Ϊselect���
	else if (temp == "select")
		cmd = select_clause(SQL, start,cmd);
	//��Ϊinsert���
	else if (temp == "insert")
		cmd = insert_clause(SQL, start,cmd);
	//��Ϊdelete���
	else if (temp == "delete")
		cmd = delete_clause(SQL, start,cmd);
	//��Ϊuse���
	else if (temp == "use")
		cmd = use_clause(SQL, start,cmd);
	//��Ϊexecfile���
	else if (temp == "execfile")
		cmd = execfile_clause(SQL, start,cmd);
	//��Ϊquit���
	else if (temp == "quit")
		cmd = quit_clause(SQL, start,cmd);
	//��ȡ����
	else if (temp == "help")
		strcpy_s(cmd.op,"60");
	//��Ϊ�Ƿ����
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//�������������ڲ���ʽ
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ�û�����
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
	//�������дת��ΪСд
	transform(SQL.begin(), SQL.end(), SQL.begin(), ::tolower);
	//�����û�����
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create database����Ƿ���Ч
command create_database(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: database name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//����create database�����ڲ���ʽ
		else
		{
			strcpy_s(cmd.op, "00");
			cmd.arg1 = temp;
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create table����Ƿ���Ч
command create_table(string SQL, int start,command cmd)
{
	string temp, sql;
	int end, length;
	//��ȡ����
	while (SQL.at(start) == ' ')
		start++;
	if ((end = SQL.find('(', start)) == -1)
	{
		cout << "syntax error: missing ( in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}
	temp = SQL.substr(start, end - start);

	//�������ǿ�
	if (!temp.empty())
	{
		//�������(ɾȥ�ո�)
		length = temp.length() - 1;
		while (temp.at(length) == ' ')
			length--;
		temp = temp.substr(0, length + 1);
		cmd.arg1 = temp;
	}

	//��ȡarg2
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
//��֤create index on����Ƿ���Ч
command create_index_on(string SQL, int start, string sql,command cmd)
{
	string temp;
	int end, length;
	//��ȡ����
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find('(', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		cout << "\t missing ( !" << endl;
		strcpy_s(cmd.op, "99");
		//return SQL;
	}
	else
	{
		//�����Ƿ�Ϊ��Ч�ļ���
		length = temp.length() - 1;
		while (temp.at(length) == ' ')
			length--;
		temp = temp.substr(0,length + 1);
		//��Ч
		if (temp.find(' ') == -1)
		{
			sql += " " + temp;
			cmd.arg2=temp;
			//��ȡ������
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(')', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for create index statement!" << endl;
				cout << "\t missing ) !" << endl;
				strcpy_s(cmd.op, "99");
				//return SQL;
			}
			else
			{
				//�����Ƿ�Ϊ��Ч������
				length = temp.length() - 1;
				while (temp.at(length) == ' ')
					length--;
				temp = temp.substr(0,length + 1);
				//��Ч
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
					//����create index�����ڲ���ʽ
					else {
						strcpy_s(cmd.op, "20");
					}
						
				}
				//��Ч,��ӡ������Ϣ
				else
				{
					cout << "error:" << " " << temp << "---is not a valid attribute name!" << endl;
					strcpy_s(cmd.op, "99");
					//return SQL;
				}
			}
		}
		//��Ч,��ӡ������Ϣ
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
//��֤create index����Ƿ���Ч
command create_index(string SQL, int start,command cmd)
{
	string temp, sql;
	int end;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		strcpy_s(cmd.op,"99");
	}
	else
	{
		sql = temp;
		cmd.arg1 = temp;
		//��ȡ���ĸ�����
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for create index statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//��Ϊon,������֤
		else if (temp == "on")
			cmd = create_index_on(SQL, start, sql,cmd);
		//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			strcpy_s(cmd.op, "99");
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create����Ƿ���Ч
command create_clause(string SQL, int start,command cmd)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//��Ϊdatabase,������֤
	else if (temp == "database")
		cmd = create_database(SQL, start,cmd);
	//��Ϊtable,������֤
	else if (temp == "table")
		cmd = create_table(SQL, start,cmd);
	//��Ϊindex,������֤
	else if (temp == "index")
		cmd = create_index(SQL, start,cmd);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//����create�����ڲ���ʽ
	return cmd;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop database����Ƿ���Ч
command drop_database(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: database name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//����drop database�����ڲ���ʽ
		else {
			strcpy_s(cmd.op, "02");
			cmd.arg1 = temp;
		}
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop table����Ƿ���Ч
command drop_table(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: table name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid table name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//����drop table�����ڲ���ʽ
		else {
			strcpy_s(cmd.op, "11");
			cmd.arg1 = temp;
		}
	}

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop index����Ƿ���Ч
command drop_index(string SQL, int start,command cmd)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: index name has not been specified!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		while (SQL.at(start) == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(start) != ';' || start != SQL.length() - 1)
		{
			cout << "syntax error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid index name!" << endl;
			strcpy_s(cmd.op, "99");
		}
		//����drop index�����ڲ���ʽ
		else {
			strcpy_s(cmd.op, "21");
			cmd.arg1 = temp;
		}
	}

	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop����Ƿ���Ч
command drop_clause(string SQL, int start,command cmd) {
	string temp;
	int end;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for drop statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//��Ϊdatabase,������֤
	else if (temp == "database")
		cmd = drop_database(SQL, start,cmd);
	//��Ϊtable,������֤
	else if (temp == "table")
		cmd = drop_table(SQL, start,cmd);
	//��Ϊindex,������֤
	else if (temp == "index")
		cmd = drop_index(SQL, start,cmd);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//����drop�����ڲ���ʽ
	return cmd;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//��֤select ����Ƿ���Ч
command select_clause(string SQL, int start,command cmd) {
	string temp, sql, table;
	int end;
	//��ȡ����
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else
	{
		cmd.arg1 = temp;
		//��ȡ����������
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for select statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		else if (temp == "from"){
			//��ȡ���ĸ�����
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: missing ; in the statement!" << endl;
				strcpy_s(cmd.op, "99");
			}
			else {
				cmd.arg2 = temp;
				//��ȡ��������ʣ��Ƿ���where��
				while (SQL.at(start) == ' ')
					start++;
				end = SQL.find(' ', start);
				temp = SQL.substr(start, end - start);
				start = end + 1;
				//���ޣ���ӡ������Ϣ
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
					//��ȡcondition��Ϣ
					end = SQL.find(';');
					while (SQL.at(start) == ' ') start++;
					temp = SQL.substr(start, end - start - 1);
					if (temp.empty()) {
						cout << "syntax error: missing condition in the statement!" << endl;
						strcpy_s(cmd.op, "99");
					}
					else cmd.arg3 = temp;
				}
				//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
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
//��֤insert ����Ƿ���Ч
command insert_clause(string SQL, int start,command cmd) {
	string temp, sql;
	int end;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for insert statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//��Ϊinto,������֤
	else if (temp == "into") {
		//��ȡ����������(����)
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for insert statement!" << endl;
			strcpy_s(cmd.op, "99");
			return cmd;
		}
		else {
			sql ="30"+ temp;
			cmd.arg1 = temp;
			//��ȡ���ĸ�����
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find('(', start);
			temp = SQL.substr(start, end - start);
			start = end;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert statement!" << endl;
				strcpy_s(cmd.op, "99");
				return cmd;
			}
			else if (temp == "values") {
				cmd = insert_into_values(SQL, start, sql,cmd);
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
				strcpy_s(cmd.op, "99");
				return cmd;
			}
		}
	}
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//����drop�����ڲ���ʽ
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//��֤insert into values����Ƿ���Ч
command insert_into_values(string SQL, int start, string sql,command cmd) {
	string temp;
	int end, length;

	if ((start = SQL.find('(', start)) == -1)
	{
		cout << "syntax error: missing ( in the statement!" << endl;
		strcpy_s(cmd.op, "99");
		return cmd;
	}

	//��ȡarg2
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
//��֤delete����Ƿ���Ч
command delete_clause(string SQL, int start,command cmd) {
	string temp, sql,table;
	int end;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for delete statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	else if (temp=="from")
	{
		//��ȡ���������ʣ�������
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for delete statement!" << endl;
			strcpy_s(cmd.op, "99");
		}
		else {
			table = temp;
			//��ȡ���ĸ�����
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
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
				//��ȡcondition��Ϣ
				end = SQL.find(';');
				while (SQL.at(start) == ' ') start++;
				temp = SQL.substr(start, end - start - 1);
				if (temp.empty()) {
					cout << "syntax error: missing condition in the statement!" << endl;
					strcpy_s(cmd.op, "99");
				}
				else cmd.arg2 = temp;
			}
			//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
			else
			{
				cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
				strcpy_s(cmd.op, "99");
			}
		}
	}
	//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//��֤use����Ƿ���Ч
command use_clause(string SQL, int start,command cmd) {
	string temp;
	int end,index;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for use statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//�������
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
//��֤execfile����Ƿ���Ч
command execfile_clause(string SQL, int start,command cmd) {
	string temp;
	int end, index;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for execfile statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//�������
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
//��֤quit����Ƿ���Ч
command quit_clause(string SQL, int start,command cmd) {
	string temp;
	int end;
	//��ȡ�ڶ�������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for quit statement!" << endl;
		strcpy_s(cmd.op, "99");
	}
	//��Ϊ;,�˳�����
	else if (temp == ";") {
		cout << "exiting program..." << endl;
		strcpy_s(cmd.op, "50");
	}
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		strcpy_s(cmd.op, "99");
	}

	return cmd;
}