#pragma once

#include "scanner.h"
#include <memory>
#include <vector>

class BinaryExpression;
class GroupingExpression;
class PrefixExpression;
class VariableExpression;

class ExpressionVisitor {
  public:
	virtual void visit(BinaryExpression *bin)     = 0;
	virtual void visit(GroupingExpression *group) = 0;
	virtual void visit(PrefixExpression *pe)      = 0;
	virtual void visit(VariableExpression *vis)   = 0;
};

class Expr {
  public:
	enum Type { BINARY, VARIABLE, GROUPING, POSTFIX };
	Token token;
	Type  type;
	Expr(Token tok, Type t) : token(tok), type(t){};
	virtual ~Expr() {}
	virtual void accept(ExpressionVisitor *visitor) = 0;
	Type         getType() { return type; }
	bool         isVariable() { return (type == VARIABLE); }
	friend class ExpressionVisitor;
};

using ExpPtr = std::unique_ptr<Expr>;

class BinaryExpression : public Expr {
  public:
	ExpPtr left, right;
	BinaryExpression(ExpPtr &l, Token op, ExpPtr &r)
	    : Expr(op, BINARY), left(l.release()), right(r.release()) {}
	void accept(ExpressionVisitor *visitor) { visitor->visit(this); }
};

class VariableExpression : public Expr {
  public:
	VariableExpression(Token t) : Expr(t, VARIABLE) {}
	void accept(ExpressionVisitor *visitor) { visitor->visit(this); }
	bool isVariable() { return true; }
};

class GroupingExpression : public Expr {
  public:
	ExpPtr exp;
	GroupingExpression(Token brace, ExpPtr &expr)
	    : Expr(brace, GROUPING), exp(expr.release()) {}
	void accept(ExpressionVisitor *visitor) { visitor->visit(this); }
};

class PrefixExpression : public Expr {
  public:
	ExpPtr left;
	PrefixExpression(ExpPtr &l, Token t)
	    : Expr(t, POSTFIX), left(l.release()) {}
	void accept(ExpressionVisitor *visitor) { visitor->visit(this); }
};

class ExpressionPrinter : public ExpressionVisitor {
  private:
	std::ostream &out;

  public:
	ExpressionPrinter(std::ostream &os);
	void print(Expr *e);
	void visit(BinaryExpression *bin);
	void visit(GroupingExpression *group);
	void visit(PrefixExpression *pe);
	void visit(VariableExpression *vis);
};
