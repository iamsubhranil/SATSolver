#include "bytecode.h"
#include "expr.h"
#include "hashmap.h"

class CodeGenerator : ExpressionVisitor {
  private:
	void visit(BinaryExpression *bin);
	void visit(GroupingExpression *group);
	void visit(PrefixExpression *pe);
	void visit(VariableExpression *vis);

	HashMap<std::string, int> slots;
	Bytecode                  code;
	int                       slotNum;

  public:
	CodeGenerator();
	Bytecode generate(ExpPtr &p);
};
