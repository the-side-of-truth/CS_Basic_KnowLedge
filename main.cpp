#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include "1_lexer.h"
#include "2_AST.h"
#include <vector>
#include <string>
using namespace std;

int main() {
	auto tokParser = TokensParser();
	tokParser.TokenParserDriver();
	return 0;
}