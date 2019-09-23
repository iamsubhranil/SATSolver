#include "codegen.h"
#include "display.h"
#include "engine.h"
#include "optimizer.h"
#include "parser.h"
#include "scanner.h"
#include <iostream>
#include <string.h>

using namespace std;

static void prefix(Parser *p, TokenType op, int prec) {
	p->registerParselet(op, new PrefixOperatorParselet(prec));
}

static void infixLeft(Parser *p, TokenType t, int prec) {
	p->registerParselet(t, new BinaryOperatorParselet(prec, false));
}

void registerParselets(Parser *p) {
	p->registerParselet(TOKEN_IDENTIFIER, new NameParselet());
	p->registerParselet(TOKEN_LEFT_PAREN, new GroupParselet());
	prefix(p, TOKEN_COMPLEMENT, Precedence::COMPLEMENT);

	infixLeft(p, TOKEN_OR, Precedence::OR);
	infixLeft(p, TOKEN_AND, Precedence::AND);
}

void solve(Scanner s) {
	Parser p(s);
	registerParselets(&p);
	ExpressionPrinter ep(cout);
	ExpPtr            expression = p.parseExpression();
#ifdef DEBUG
	ep.print(expression.get());
	cout << "\n";
#endif
	CodeGenerator c;
	Bytecode      b = c.generate(expression);
#ifdef DEBUG
	b->disassemble();
	cout << "\nGenerating solutions..\n";
#endif
	// cout << "\nOptimizing..\n";
	// Optimizer::optimize(b);
	// b->disassemble();
	clock_t start = clock();
	Engine::solve(b);
	clock_t end = clock() - start;
	cout << "\nelapsed: " << ((double)end / CLOCKS_PER_SEC) << "";
}

int main(int argc, char **argv) {
	if(argc < 2) {
		err("Give the name of the file, or use '%s -g N' to randomly "
		    "generate and solve an SAT of N terms.",
		    argv[0]);
		return 1;
	}
	/*
	if(argc == 3 && strcmp(argv[1], "-g") == 0) {
	    char *end       = NULL;
	    long  noOfTerms = strtol(argv[1], &end, 10);
	    if(*end != '\0') {
	        err("Bad number of terms '%s'!", argv[1]);
	        return 2;
	    }
	    char *source = generate_sat(noOfTerms);
	} else*/
	if(argc == 2) {
		Scanner s(argv[1]);
		solve(s);
	} else {
		err("Give the name of the file, or use '%s -g N' to randomly "
		    "generate and solve an SAT of N terms.",
		    argv[0]);
		return 1;
	}
}