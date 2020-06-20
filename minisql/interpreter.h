#ifndef _INTERPRETER_H
#define _INTERPRETER_H


/* �� interpreter ģ�飺
 * api ģ���ж����ݱ����ݿ⡢��������Ҫ������(����ͬ mySQL ��Ҫ��)��
 * 1. ����ֻ�ܰ������֡���Сд��ĸ���»��ߣ�
 * 2. û������ĸ����Ϊ���ֵ�Ҫ��
 * ϣ�����﷨���ʱע�⡣
 */

// Ŀǰ����ֻ���г��� api ģ����Ҫ�����ݵĸ�ʽ��

#include <string>
using namespace std;

// api ģ������Ҫ������ĸ�ʽ��
// op Ϊ2λ���֣�������������͡�
// arg1..Ϊ��������ݡ�������ݲ�ͬ����������� executeCommand ������
// ע�⣺��ʱ��Ҫ��֤���� arg �����ݲ������л��С�����Ӧ���� intrepreter �Ѿ�������ġ�
struct command {
	char op[3];  // ��������͡�
	string arg1; // ����ĵ�һ��������
	string arg2; // ����ĵڶ���������
	string arg3; // ����ĵ�����������
};

#endif