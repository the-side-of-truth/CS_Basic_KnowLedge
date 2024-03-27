#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <vector>
#include <string>

/*
* 万花筒(kaleidoscope)语言的词法分析器
* 只有一种数据类型double，没有类型识别的必要
* 输入是一个文本文件
* 为当前Identifier或NumVal赋值并返回对应类型，关键字也返回对应类型，其余符号则返回acsii码的码值
*/

class Lexer {
public:
	// 构造函数和析构函数
	Lexer();
	~Lexer();

	// 定义一些关键字和返回规则
	static enum Token {
		tok_error = -6, // 标志着单词格式错误

		tok_eof = -1, // 文件结束符

		tok_def = -2, // 函数定义关键字
		tok_extern = -3, // 外部声明关键字

		tok_identifier = -4, // 标识符
		tok_number = -5, // 数值
	};

	// 从缓冲区读取数据
	int getNextToken();

	/*-------实用函数-------*/
	// 判断一个字符是否是字母
	bool is_al(char c);

	// 判断一个字符是否是数字
	bool is_num(char c);

	/*获取私有属性函数*/
	std::string getIdentifier();
	double getNumVal();
	char getNext_c();
	int getToktype();// 获取当前持有的token的类型
	
private:
	std::string IdentifierStr; // 标识符的内容
	double NumVal; // 数值
	char next_c = ' ';// 下一个字符(自用)
	char tok_type;// 当前解析的token的类型
};

// 一个简单的Lexer驱动
void simple_lexer_driver();

