#include "codegen.h"
#include "display.h"

using namespace std;

CodeGenerator::CodeGenerator() {
	slots.clear();
}

Bytecode CodeGenerator::generate(ExpPtr &p) {
	code = make_unique<BytecodeHolder>();
	slots.clear();
	slotNum = 0;

	p->accept(this);
	code->halt();

	return Bytecode(code.release());
}

void CodeGenerator::visit(VariableExpression *vis) {
	string n = string(vis->token.start, vis->token.length);
	if(slots.find(n) == slots.end()) {
		slots[n] = slotNum++;
		code->insertSlot();
	}
	code->load_slot_n(slots[n]);
}

void CodeGenerator::visit(BinaryExpression *bi) {
	bi->left->accept(this);

	switch(bi->token.type) {
		case TOKEN_OR: {
			int pos = code->jumpiftrue(0);
			bi->right->accept(this);
			code->or_();
			code->jumpiftrue(pos, code->getip() - pos);
			break;
		}
		case TOKEN_AND: {
			int pos = code->jumpiffalse(0);
			bi->right->accept(this);
			code->and_();
			code->jumpiffalse(pos, code->getip() - pos);
			break;
		}
		default:
			panic("Invalid binary operator '%s'!",
			      Token::FormalNames[bi->token.type]);
	}
}

void CodeGenerator::visit(GroupingExpression *g) {
	g->exp->accept(this);
}

void CodeGenerator::visit(PrefixExpression *p) {
	p->left->accept(this);
	switch(p->token.type) {
		case TOKEN_COMPLEMENT: code->complement(); break;
		default:
			panic("Invalid prefix operator '%s'!",
			      Token::FormalNames[p->token.type]);
	}
}
