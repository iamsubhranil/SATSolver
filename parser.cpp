#include "parser.h"

Parser::Parser(Scanner &s) : scanner(s) {
	prefixParselets.clear();
	infixParselets.clear();
	tokenCache.clear();
}

void Parser::registerParselet(TokenType t, PrefixParselet *p) {
	prefixParselets[t] = PrefixParseletPtr(p);
}

void Parser::registerParselet(TokenType t, InfixParselet *i) {
	infixParselets[t] = InfixParseletPtr(i);
}

ExpPtr Parser::parseExpression(int precedence, Token token) {
	PrefixParselet *prefix = prefixParselets[token.type].get();

	if(prefix == NULL) {
		throw ParseException(token, "Unable to parse the expression!");
	}

	ExpPtr left = prefix->parse(this, token);

	while(precedence < getPrecedence()) {
		token = lookAhead(0);

		InfixParselet *infix = infixParselets[token.type].get();
		token                = consume();
		left                 = infix->parse(this, left, token);
	}

	return left;
}

ExpPtr Parser::parseExpression(Token token) {
	return parseExpression(0, token);
}

ExpPtr Parser::parseExpression(int precedence) {
	return parseExpression(precedence, consume());
}

ExpPtr Parser::parseExpression() {
	return parseExpression(0);
}

Token Parser::lookAhead(size_t distance) {
	while(distance >= tokenCache.size()) {
		tokenCache.push_back(scanner.scanNextToken());
	}

	return tokenCache.at(distance);
}

Token Parser::consume() {
	lookAhead(0);

	Token t = tokenCache.front();
	tokenCache.pop_front();
	return t;
}

Token Parser::consume(TokenType t, const char *message) {
	Token token = lookAhead(0);
	if(token.type != t) {
		if(token.type == TOKEN_EOF) {
			throw ParseException(token, "Unexpected end of file!");
		}
		throw ParseException(token, message);
	}
	return consume();
}

bool Parser::match(TokenType t) {
	Token token = lookAhead(0);
	if(token.type != t) {
		return false;
	}
	consume();
	return true;
}

int Parser::getPrecedence() {
	InfixParselet *p = infixParselets[lookAhead(0).type].get();
	if(p != NULL)
		return p->getPrecedence();
	return 0;
}

// Expressions

ExpPtr NameParselet::parse(Parser *parser, Token t) {
	(void)parser;
	return unq(VariableExpression, t);
}

ExpPtr GroupParselet::parse(Parser *parser, Token t) {
	ExpPtr expr = parser->parseExpression();
	parser->consume(TOKEN_RIGHT_PAREN,
	                "Expected ')' at the end of the group expression!");
	return unq(GroupingExpression, t, expr);
}

ExpPtr BinaryOperatorParselet::parse(Parser *parser, ExpPtr &left, Token t) {
	ExpPtr right = parser->parseExpression(getPrecedence() - isRight);
	return unq(BinaryExpression, left, t, right);
}

ExpPtr PrefixOperatorParselet::parse(Parser *parser, Token t) {
	ExpPtr left = parser->parseExpression(getPrecedence());
	return unq(PrefixExpression, left, t);
}
