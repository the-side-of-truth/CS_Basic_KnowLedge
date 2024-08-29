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
	if (BinopPrecedence.count(op) == 0) return -1; // 最低优先级，非运算符
	else return BinopPrecedence[op];
}

std::unique_ptr<ExprAST>TokensParser::NumberParser() {
	auto result = std::make_unique<NumberExprAST>(lexer.getNumVal());
	lexer.getNextToken();
	return std::move(result);
}

std::unique_ptr<ExprAST> TokensParser::ParenParser() {
	lexer.getNextToken(); // 吃掉'('
	auto result = ExprParser(); // 获得一个表达式
	if (result == nullptr) return nullptr;
	if (lexer.getNextToken() != ')') {
		LogError("Mismatch or lack a ')'.");
	}
	lexer.getNextToken();
	return std::move(result);
}

std::unique_ptr<ExprAST> TokensParser::IdentifierParser() {
	// 接收一个标识符
	auto IdExpr = make_unique<VariableExprAST>(lexer.getIdentifier());
	std::string Id_name = lexer.getIdentifier();
	char c = lexer.getNextToken();// 吃掉当前标识符
	if (c != '(') {
		// 不是一个函数调用
		return std::move(IdExpr);
	}
	// 开始解析函数调用
	lexer.getNextToken();// 吃掉'('
	std::vector<std::unique_ptr<ExprAST> > Args;
	if (lexer.getToktype() == ')') {
		// 函数无参数,表达式解析结束
		lexer.getNextToken(); // 吃掉')'
		return std::make_unique<CallExprAST>(Id_name, std::move(Args));
	}
	while (true) {
		auto temp_expr = ExprParser();
		if (temp_expr == nullptr) {
			LogError("Expext a expression.");
		}
		Args.push_back(std::move(temp_expr));

		if (lexer.getToktype() == ',') {
			// 英文逗号表示继续解读
			lexer.getNextToken();// 吃掉','
			continue;
		}
		else if (lexer.getToktype() == ')') {
			// 表达式解析结束
			lexer.getNextToken(); // 吃掉')'
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
		if (cur_pre_level <= left_op_level) return std::move(LHS); // 优先级不足
		lexer.getNextToken();// 吃掉运算符
		std::unique_ptr<ExprAST> RHS = PrimaryParser();// 看右边的表达式
		if (!RHS) return nullptr;
		// 判断是否继续往下扫描,RHS归属权
		int next_op_level = getBinaryPrecedence(lexer.getToktype());
		if (cur_pre_level < next_op_level) {
			// RHS归属右边，继续扫描
			RHS = BinaryExprParser(cur_pre_level, std::move(RHS));
			if (!RHS) return nullptr;
		}
		// RHS归属左边，整合
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

	// 解析参数
	std::vector<std::string> args;
	while (lexer.getNextToken() == lexer.tok_identifier) {
		args.push_back(lexer.getIdentifier());
	}
	if (lexer.getToktype() != ')') {
		return LogErrorP("Expected ')' in prototype");
	}
	lexer.getNextToken(); // 吃掉')'
	return std::make_unique<PrototypeAST>(FNname, std::move(args));
	//return nullptr;
}

std::unique_ptr<FunctionAST> TokensParser::FunctionParser() {
	lexer.getNextToken(); // 吃掉 def
	// 解析一个函数原型
	auto FunPrototype = PrototypeParser();
	if (!FunPrototype) {
		return nullptr;
	}
	// 解析一个表达式
	auto E = ExprParser();
	if (!E) {
		return nullptr;
	}
	return std::make_unique<FunctionAST>(std::move(FunPrototype), std::move(E));
}

std::unique_ptr<PrototypeAST> TokensParser::ExternParser() {
	lexer.getNextToken(); // 吃掉extern
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

