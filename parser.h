#pragma once

#include "expr.h"
#include "hashmap.h"
#include <deque>

class Precedence {
	// Ordered in increasing precedence.
  public:
	static const int OR         = 2;
	static const int AND        = 3;
	static const int COMPLEMENT = 4;
	static const int PRIMARY    = 13;
};

class Parser;

class PrefixParselet {
  public:
	virtual ExpPtr parse(Parser *parser, Token t) = 0;
	virtual ~PrefixParselet() {}
};

class NameParselet : public PrefixParselet {
  public:
	ExpPtr parse(Parser *parser, Token t);
};

class PrefixOperatorParselet : public PrefixParselet {
  private:
	int precedence;

  public:
	PrefixOperatorParselet(int prec) : precedence(prec) {}
	ExpPtr parse(Parser *parser, Token t);
	int    getPrecedence() { return precedence; }
};

class GroupParselet : public PrefixParselet {
  public:
	ExpPtr parse(Parser *parser, Token t);
};

class InfixParselet {
  private:
	int precedence;

  public:
	InfixParselet(int prec) : precedence(prec) {}
	virtual ExpPtr parse(Parser *parser, ExpPtr &left, Token t) = 0;
	int            getPrecedence() { return precedence; }
	virtual bool   isAssignment() { return false; }
	virtual ~InfixParselet() {}
};

class BinaryOperatorParselet : public InfixParselet {
  private:
	bool isRight;

  public:
	BinaryOperatorParselet(int precedence, bool isr)
	    : InfixParselet(precedence), isRight(isr) {}
	ExpPtr parse(Parser *parser, ExpPtr &left, Token t);
};

// Parser

using PrefixParseletPtr = std::unique_ptr<PrefixParselet>;
using InfixParseletPtr  = std::unique_ptr<InfixParselet>;

class Parser {
  private:
	HashMap<TokenType, PrefixParseletPtr> prefixParselets;
	HashMap<TokenType, InfixParseletPtr>  infixParselets;
	Scanner &                             scanner;
	std::deque<Token>                     tokenCache;

	int getPrecedence();

  public:
	Parser(Scanner &sc);
	Token  lookAhead(size_t distance);
	bool   match(TokenType expected);
	Token  consume();
	Token  consume(TokenType expected, const char *message);
	void   registerParselet(TokenType type, PrefixParselet *p);
	void   registerParselet(TokenType type, InfixParselet *p);
	ExpPtr parseExpression(Token token);
	ExpPtr parseExpression();
	ExpPtr parseExpression(int precedence, Token token);
	ExpPtr parseExpression(int precedence);
};

class ParseException : public std::runtime_error {
  private:
	Token       t;
	const char *message;

  public:
	ParseException() : runtime_error("Error occurred while parsing!") {}
	ParseException(Token to, const char *msg)
	    : runtime_error(msg), t(to), message(msg) {}
	virtual const char *what() const throw() {
		t.highlight();
		return message;
	}
};
