#pragma once

#include "bytecode.h"

class Engine {
  public:
	static void next_sequence(bool *sq, int num);
	static void print_sequence(bool *sq, int num);
	static void solve(Bytecode &code);
	static bool execute(const unsigned char *ins, bool *stack_, int slots);
};
