#include "2_AST.h"

using namespace std;

TokensParser::TokensParser() {
	this->lexer = Lexer();
}

TokensParser::~TokensParser() {

}

std::unique_ptr<ExprAST> TokensParser::LogError(const char* Str) {
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> TokensParser::LogErrorP(const char* Str) {
	LogError(Str);
	return nullptr;
}

int TokensParser::getBinaryPrecedence(char op) {
	if (BinopPrecedence.count(op) == 0) return -1; // ������ȼ����������
	else return BinopPrecedence[op];
}

std::unique_ptr<ExprAST>TokensParser::NumberParser() {
	auto result = std::make_unique<NumberExprAST>(lexer.getNumVal());
	lexer.getNextToken();
	return std::move(result);
}

std::unique_ptr<ExprAST> TokensParser::ParenParser() {
	lexer.getNextToken(); // �Ե�'('
	auto result = ExprParser(); // ���һ�����ʽ
	if (result == nullptr) return nullptr;
	if (lexer.getNextToken() != ')') {
		LogError("Mismatch or lack a ')'.");
	}
	lexer.getNextToken();
	return std::move(result);
}

std::unique_ptr<ExprAST> TokensParser::IdentifierParser() {
	// ����һ����ʶ��
	auto IdExpr = make_unique<VariableExprAST>(lexer.getIdentifier());
	std::string Id_name = lexer.getIdentifier();
	char c = lexer.getNextToken();// �Ե���ǰ��ʶ��
	if (c != '(') {
		// ����һ����������
		return std::move(IdExpr);
	}
	// ��ʼ������������
	lexer.getNextToken();// �Ե�'('
	std::vector<std::unique_ptr<ExprAST> > Args;
	if (lexer.getToktype() == ')') {
		// �����޲���,���ʽ��������
		lexer.getNextToken(); // �Ե�')'
		return std::make_unique<CallExprAST>(Id_name, std::move(Args));
	}
	while (true) {
		auto temp_expr = ExprParser();
		if (temp_expr == nullptr) {
			LogError("Expext a expression.");
		}
		Args.push_back(std::move(temp_expr));

		if (lexer.getToktype() == ',') {
			// Ӣ�Ķ��ű�ʾ�������
			lexer.getNextToken();// �Ե�','
			continue;
		}
		else if (lexer.getToktype() == ')') {
			// ���ʽ��������
			lexer.getNextToken(); // �Ե�')'
			return std::make_unique<CallExprAST>(Id_name, std::move(Args));
		}
		else {
			LogError("Expext a ',' or ')'");
		}

	}
}

std::unique_ptr<ExprAST> TokensParser::PrimaryParser() {
	int tok_type = lexer.getToktype();
	if (tok_type == lexer.tok_number) {
		return NumberParser();
	}
	else if (tok_type == lexer.tok_identifier) {
		return IdentifierParser();
	}
	else if (tok_type == '(') {
		return ParenParser();
	}
	return LogError("unknown token when expecting an expression");
}

std::unique_ptr<ExprAST> TokensParser::BinaryExprParser(int left_op_level, std::unique_ptr<ExprAST> LHS) {
	while (true) {
		char cur_op = lexer.getToktype();
		int cur_pre_level = getBinaryPrecedence(lexer.getToktype());
		if (cur_pre_level <= left_op_level) return std::move(LHS); // ���ȼ�����
		lexer.getNextToken();// �Ե������
		std::unique_ptr<ExprAST> RHS = PrimaryParser();// ���ұߵı��ʽ
		if (!RHS) return nullptr;
		// �ж��Ƿ��������ɨ��,RHS����Ȩ
		int next_op_level = getBinaryPrecedence(lexer.getToktype());
		if (cur_pre_level < next_op_level) {
			// RHS�����ұߣ�����ɨ��
			RHS = BinaryExprParser(cur_pre_level, std::move(RHS));
			if (!RHS) return nullptr;
		}
		// RHS������ߣ�����
		LHS = make_unique<BinaryExprAST>(cur_op, std::move(LHS), std::move(RHS));
	}
}

std::unique_ptr<ExprAST> TokensParser::ExprParser() {
	auto result = PrimaryParser();
	if (!result) return nullptr;
	return BinaryExprParser(0, std::move(result));
}

std::unique_ptr<PrototypeAST> TokensParser::PrototypeParser() {
	int cur_tok_type = lexer.getToktype();
	if (cur_tok_type != lexer.tok_identifier) return LogErrorP("Expected function name in prototype");

	std::string FNname = lexer.getIdentifier();
	lexer.getNextToken();

	if (lexer.getToktype() != '(') {
		return LogErrorP("Expected '(' in prototype.");
	}

	// ��������
	std::vector<std::string> args;
	while (lexer.getNextToken() == lexer.tok_identifier) {
		args.push_back(lexer.getIdentifier());
	}
	if (lexer.getToktype() != ')') {
		return LogErrorP("Expected ')' in prototype");
	}
	lexer.getNextToken(); // �Ե�')'
	return std::make_unique<PrototypeAST>(FNname, std::move(args));
	//return nullptr;
}

std::unique_ptr<FunctionAST> TokensParser::FunctionParser() {
	lexer.getNextToken(); // �Ե� def
	// ����һ������ԭ��
	auto FunPrototype = PrototypeParser();
	if (!FunPrototype) {
		return nullptr;
	}
	// ����һ�����ʽ
	auto E = ExprParser();
	if (!E) {
		return nullptr;
	}
	return std::make_unique<FunctionAST>(std::move(FunPrototype), std::move(E));
}

std::unique_ptr<PrototypeAST> TokensParser::ExternParser() {
	lexer.getNextToken(); // �Ե�extern
	return PrototypeParser();
}

std::unique_ptr<FunctionAST> TokensParser::TopLevelExprParser() {
	if (auto E = ExprParser()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	}
	return nullptr;
}

void TokensParser::HandleDefinition() {
	if (FunctionParser()) {
		fprintf(stderr, "Parsed a function definition.\n");
	}
	else {
		lexer.getNextToken();
	}
}

void TokensParser::HandleExtern() {
	if (ExternParser()) {
		fprintf(stderr, "Parsed a extern.\n");
	}
	else {
		lexer.getNextToken();
	}
}

void TokensParser::HandleTopLevelExpression() {
	if (TopLevelExprParser()) {
		fprintf(stderr, "Parsed a top-lexel expr.\n");
	}
	else {
		lexer.getNextToken();
	}
}

void TokensParser::TokenParserDriver() {
	// Prime the first token.
	fprintf(stderr, "ready> ");
	lexer.getNextToken();
	while (true) {
		fprintf(stderr, "ready>");
		switch (lexer.getToktype())
		{
		case Lexer::tok_eof:
			return;
		case ';':
			lexer.getNextToken();
			break;
		case '\n':
			lexer.getNextToken();
			break;
		case Lexer::tok_def:
			HandleDefinition();
			break;
		case Lexer::tok_extern:
			HandleExtern();
			break;
		default:
			HandleTopLevelExpression();
			break;
		}
	}
}

