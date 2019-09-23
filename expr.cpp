#include "expr.h"

using namespace std;

ExpressionPrinter::ExpressionPrinter(ostream &os) : out(os) {}

void ExpressionPrinter::print(Expr *e) {
	e->accept(this);
}

void ExpressionPrinter::visit(BinaryExpression *be) {
	out << "(";
	be->left->accept(this);
	out << " " << Token::FormalNames[be->token.type] << " ";
	be->right->accept(this);
	out << ")";
}

void ExpressionPrinter::visit(GroupingExpression *ge) {
	out << "( ";
	ge->exp->accept(this);
	out << " )";
}

void ExpressionPrinter::visit(PrefixExpression *pe) {
	out << pe->token;
	out << "(";
	pe->left->accept(this);
	out << ")";
}

void ExpressionPrinter::visit(VariableExpression *v) {
	out << v->token;
}
