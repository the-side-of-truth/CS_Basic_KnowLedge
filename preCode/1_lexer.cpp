#include "1_lexer.h"

Lexer::Lexer() {};
Lexer::~Lexer() {};

bool Lexer::is_al(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::is_num(char c) {
	return c >= '0' && c <= '9';
}

int Lexer::getNextToken() {
	// 首先把空格,回车全吃掉
	while (isspace(next_c)) next_c = getchar();

	// 分类
	tok_type = -1;
	if (next_c == EOF) return -1;// 文件结束符

	// 字母，数字，其他字符(需要区分'.',以点开头也是数字合法格式
	// 读数字
	if (is_num(next_c) || next_c == '.') { // 从缓冲区读一个10进制无符号数
		NumVal = 0;
		std::string number = "";
		int dot_num = 0;
		while (is_num(next_c) || next_c == '.') {
			if (next_c == '.') dot_num++; 
			number += next_c;
			if (dot_num == 2) return tok_error; // 最多一个小数点
			next_c = getchar();
		}

		// todo:找出那些数字后面不可能接的符号
		NumVal = atof(number.c_str());
		tok_type = tok_number;
		return tok_number;
	}

	// 读标识符和关键字
	if (is_al(next_c)) {// 读一个标识符,必须以字母开头，后面可跟随字母或数字，不支持下划线。需要识别出关键字
		/*当前的关键字
		def,extern
		*/
		IdentifierStr = next_c;
		while (true) {
			next_c = getchar();
			if (!is_al(next_c) && !is_num(next_c)) break;
			IdentifierStr += next_c;
		}

		
		if (IdentifierStr.compare("def") == 0) {
			tok_type = tok_def;
			return tok_def;
		}
		if (IdentifierStr.compare("extern") == 0) {
			tok_type = tok_extern;
			return tok_extern;
		}

		// todo:找出标识符后不可能接的符号
		tok_type = tok_identifier;
		return tok_identifier;
	}

	// 读注释
	if (next_c == '#') {
		while (next_c != '\n' && next_c != '\r' && next_c != EOF) next_c = getchar();
		if (next_c != EOF) return getNextToken();
	}

	char temp_c = next_c;
	next_c = getchar();
	tok_type = temp_c;
	return temp_c;// 其他符号
}

std::string Lexer::getIdentifier() {
	return this->IdentifierStr;
}

double Lexer::getNumVal() {
	return this->NumVal;
}

char Lexer::getNext_c() {
	return next_c;
}

int Lexer::getToktype() {
	return tok_type;
}

void simple_lexer_driver() {
	using namespace std;
	vector<pair<int, string> > res;
	Lexer lexer;
	int tok_type;
	do {
		tok_type = lexer.getNextToken();
		if (tok_type == lexer.tok_number) {
			res.push_back(make_pair(lexer.tok_number, to_string(lexer.getNumVal())));
		}
		else if (tok_type < 0) {
			res.push_back(make_pair(tok_type, lexer.getIdentifier()));
		}
		else {
			char c = tok_type;
			string s = " ";
			s[0] = c;
			res.push_back(make_pair(tok_type, s));
		}
	} while (lexer.getNext_c() != '\n');
	for (auto& info : res) {
		cout << "类型:" << info.first << "\t" << "信息:" << info.second << endl;
	}
}