/*声明抽象语法树(AST)所使用的数据结构、主类和驱动*/
#pragma once
#include "1_lexer.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "tools.h"

using namespace llvm;
/*--------------------数据结构定义------------------------*/
// 树节点基类
class ExprAST {
public:
	virtual ~ExprAST() = default;
};

// 数值类节点
class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double val) : Val(val) {};
	//Value* codegen() override;
};

// 变量（标识符）类节点
class VariableExprAST : public ExprAST {
	std::string Name;
public:
	VariableExprAST(const std::string& name) : Name(name) {};
};

// 函数调用类语句节点
class CallExprAST : public ExprAST {
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST> > Args;
public:
	CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST> > Args) : 
		Callee(Callee), Args(std::move(Args)) {};
};

// 二元运算表达式类节点
class BinaryExprAST : public ExprAST {
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;
public:
	BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
		Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {};
};

// 函数原型（声明）类节点
class PrototypeAST {
	std::string Name;
	std::vector<std::string> Args;
public:
	PrototypeAST(const std::string& name, std::vector<std::string> args) : Name(name), Args(args) {};
	const std::string& getName() const { return Name; }
};

// 完整函数类节点
class FunctionAST {
	std::unique_ptr<PrototypeAST> Prototype;
	std::unique_ptr<ExprAST> Body;
public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) :
		Prototype(std::move(prototype)), Body(std::move(body)) {};
};


/*--------------------Token流解析类（主类)声明------------------------*/
class TokensParser {
private:
	Lexer lexer;// 词法分析器
	std::map<char, int> BinopPrecedence = {// 二元运算符优先级
		{'<', 10},
		{'+', 20},
		{'-', 20},
		{'*', 40}
	};

public:
	// 构造和析构
	TokensParser();// 构造函数
	~TokensParser();

	// 报错函数
	std::unique_ptr<ExprAST> LogError(const char* Str);
	// 报错函数
	std::unique_ptr<PrototypeAST> LogErrorP(const char* Str);

	// 字符运算优先级获取
	int getBinaryPrecedence(char op);

	/*----------------------------解析器--------------------------*/
	// 解析数值token
	std::unique_ptr<ExprAST> NumberParser(); 

	// 解析括号 格式：(ExprAST)
	std::unique_ptr<ExprAST> ParenParser();

	// 解析标识符token,顺便解析函数调用, id | id( (expr ,)* )
	std::unique_ptr<ExprAST> IdentifierParser();

	// 解析基础(叶子）类型,包括数值、括号、标识符和函数调用
	std::unique_ptr<ExprAST> PrimaryParser();

	// 解析二元表达式, priexpr (op priexpr)*
	std::unique_ptr<ExprAST> BinaryExprParser(int left_op_level, std::unique_ptr<ExprAST> LHS);

	// 解析表达式汇总
	std::unique_ptr<ExprAST> ExprParser();

	// 解析函数原型, def id(id*)
	std::unique_ptr<PrototypeAST> PrototypeParser();

	// 解析完全函数
	std::unique_ptr<FunctionAST> FunctionParser();

	// 解析extern
	std::unique_ptr<PrototypeAST> ExternParser();

	// 解析匿名函数
	std::unique_ptr<FunctionAST> TopLevelExprParser();

	// --------------------驱动(按照token类型分类)------------------
	// 解析函数定义
	void HandleDefinition();

	// 解析extern语句
	void HandleExtern();

	// 解析表达式
	void HandleTopLevelExpression();

	// 驱动主体
	void TokenParserDriver();
};

/*--------------------驱动声明------------------------*/
