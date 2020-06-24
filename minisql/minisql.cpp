#include"minisql.h"
#include"Interpreter.h"
#include"api.h"
#include <fstream>

using namespace std;

void txtreader(string filename) {
	ifstream fin(filename);
	//判断文件是否打开成功
	if (!fin.is_open()) {
		cout << "open file error" << endl;
		return;
	}

	string SQL,temp,str;	
	while (!fin.eof()) {
		bool finish = false;
		SQL.clear();
		str.clear();
		while (!finish)
		{
			fin >> str;
			if (fin.eof()) {
				fin.close();
				return;
			}
			temp = str;
			SQL = SQL + " " + temp;
			if (SQL.at(SQL.length() - 1) == ';')
			{
				SQL[SQL.length() - 1] = ' ';
				SQL += ";";
				finish = true;
			}
		}
		//将输入大写转化为小写
		transform(SQL.begin(), SQL.end(), SQL.begin(), ::tolower);
		command cmd;
		cmd = Interpreter(SQL);
		try {
			executeCommand(cmd);
		}
		catch (SqlError e) {
			e.printError();
		}
		//cout << "op: " << cmd.op << endl;
		//cout << "arg1: " << cmd.arg1 << endl;
		//cout << "arg2: " << cmd.arg2 << endl;
		//cout << "arg3: " << cmd.arg3 << endl;
	}

	fin.close();
}

void main()
{
	string SQL;
	command cmd;
	//打印软件信息
	cout << endl;
	cout << "\t\t***********************************************" << endl << endl;
	cout << "\t\t             欢迎使用 MiniSQL !" << endl;
	cout << "\t\t               Version (1.0)  " << endl;
	cout << "\t\t            " << endl;
	cout << "\t\t***********************************************" << endl;
	cout << endl << endl;
	while (1)
	{
		cout << "MiniSQL-->> ";
		SQL.clear();
		cmd = Interpreter(SQL);
		//cout << "op: " << cmd.op << endl;
		//cout << "arg1: " << cmd.arg1 << endl;
		//cout << "arg2: " << cmd.arg2 << endl;
		//cout << "arg3: " << cmd.arg3 << endl;
		if (strcmp(cmd.op,"40")==0) {
			txtreader(cmd.arg1);
		}
		else if (strcmp(cmd.op, "50") == 0) {
			exit(0);
		}
		else if (strcmp(cmd.op, "60") == 0) {
			cout << "help:" << endl;
		}
		else if (strcmp(cmd.op, "99") == 0) {
			cout << "please enter the correct command: " << endl;
		}
		else {
			try {
                executeCommand(cmd);
			}
			catch (SqlError e) {
				e.printError();
			}
		}
	}

}

