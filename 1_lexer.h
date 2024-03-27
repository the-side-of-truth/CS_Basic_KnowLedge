#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <vector>
#include <string>

/*
* ��Ͳ(kaleidoscope)���ԵĴʷ�������
* ֻ��һ����������double��û������ʶ��ı�Ҫ
* ������һ���ı��ļ�
* Ϊ��ǰIdentifier��NumVal��ֵ�����ض�Ӧ���ͣ��ؼ���Ҳ���ض�Ӧ���ͣ���������򷵻�acsii�����ֵ
*/

class Lexer {
public:
	// ���캯������������
	Lexer();
	~Lexer();

	// ����һЩ�ؼ��ֺͷ��ع���
	static enum Token {
		tok_error = -6, // ��־�ŵ��ʸ�ʽ����

		tok_eof = -1, // �ļ�������

		tok_def = -2, // ��������ؼ���
		tok_extern = -3, // �ⲿ�����ؼ���

		tok_identifier = -4, // ��ʶ��
		tok_number = -5, // ��ֵ
	};

	// �ӻ�������ȡ����
	int getNextToken();

	/*-------ʵ�ú���-------*/
	// �ж�һ���ַ��Ƿ�����ĸ
	bool is_al(char c);

	// �ж�һ���ַ��Ƿ�������
	bool is_num(char c);

	/*��ȡ˽�����Ժ���*/
	std::string getIdentifier();
	double getNumVal();
	char getNext_c();
	int getToktype();// ��ȡ��ǰ���е�token������
	
private:
	std::string IdentifierStr; // ��ʶ��������
	double NumVal; // ��ֵ
	char next_c = ' ';// ��һ���ַ�(����)
	char tok_type;// ��ǰ������token������
};

// һ���򵥵�Lexer����
void simple_lexer_driver();

