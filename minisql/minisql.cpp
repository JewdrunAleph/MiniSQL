// minisql.cpp : 程序的主入口，负责总体的处理逻辑。
//

#include "minisql.h"

#include <iostream>
#include <string>
using namespace std;

int main()
{
    // 这里是程序的控制流程，由 Interpreter 模块负责人进行编写。

	// 初始化各模块。

	try {
		// 每一次输入sql语句后处理的内容请放在 try 代码块内。
	}
	catch (SqlError e)
	{
		e.printError();
	}
}
