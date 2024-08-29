/*���������﷨��(AST)��ʹ�õ����ݽṹ�����������*/
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
/*--------------------���ݽṹ����------------------------*/
// ���ڵ����
class ExprAST {
public:
	virtual ~ExprAST() = default;
};

// ��ֵ��ڵ�
class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double val) : Val(val) {};
	//Value* codegen() override;
};

// ��������ʶ������ڵ�
class VariableExprAST : public ExprAST {
	std::string Name;
public:
	VariableExprAST(const std::string& name) : Name(name) {};
};

// �������������ڵ�
class CallExprAST : public ExprAST {
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST> > Args;
public:
	CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST> > Args) : 
		Callee(Callee), Args(std::move(Args)) {};
};

// ��Ԫ������ʽ��ڵ�
class BinaryExprAST : public ExprAST {
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;
public:
	BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
		Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {};
};

// ����ԭ�ͣ���������ڵ�
class PrototypeAST {
	std::string Name;
	std::vector<std::string> Args;
public:
	PrototypeAST(const std::string& name, std::vector<std::string> args) : Name(name), Args(args) {};
	const std::string& getName() const { return Name; }
};

// ����������ڵ�
class FunctionAST {
	std::unique_ptr<PrototypeAST> Prototype;
	std::unique_ptr<ExprAST> Body;
public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) :
		Prototype(std::move(prototype)), Body(std::move(body)) {};
};


/*--------------------Token�������ࣨ����)����------------------------*/
class TokensParser {
private:
	Lexer lexer;// �ʷ�������
	std::map<char, int> BinopPrecedence = {// ��Ԫ��������ȼ�
		{'<', 10},
		{'+', 20},
		{'-', 20},
		{'*', 40}
	};

public:
	// ���������
	TokensParser();// ���캯��
	~TokensParser();

	// ������
	std::unique_ptr<ExprAST> LogError(const char* Str);
	// ������
	std::unique_ptr<PrototypeAST> LogErrorP(const char* Str);

	// �ַ��������ȼ���ȡ
	int getBinaryPrecedence(char op);

	/*----------------------------������--------------------------*/
	// ������ֵtoken
	std::unique_ptr<ExprAST> NumberParser(); 

	// �������� ��ʽ��(ExprAST)
	std::unique_ptr<ExprAST> ParenParser();

	// ������ʶ��token,˳�������������, id | id( (expr ,)* )
	std::unique_ptr<ExprAST> IdentifierParser();

	// ��������(Ҷ�ӣ�����,������ֵ�����š���ʶ���ͺ�������
	std::unique_ptr<ExprAST> PrimaryParser();

	// ������Ԫ���ʽ, priexpr (op priexpr)*
	std::unique_ptr<ExprAST> BinaryExprParser(int left_op_level, std::unique_ptr<ExprAST> LHS);

	// �������ʽ����
	std::unique_ptr<ExprAST> ExprParser();

	// ��������ԭ��, def id(id*)
	std::unique_ptr<PrototypeAST> PrototypeParser();

	// ������ȫ����
	std::unique_ptr<FunctionAST> FunctionParser();

	// ����extern
	std::unique_ptr<PrototypeAST> ExternParser();

	// ������������
	std::unique_ptr<FunctionAST> TopLevelExprParser();

	// --------------------����(����token���ͷ���)------------------
	// ������������
	void HandleDefinition();

	// ����extern���
	void HandleExtern();

	// �������ʽ
	void HandleTopLevelExpression();

	// ��������
	void TokenParserDriver();
};

/*--------------------��������------------------------*/
