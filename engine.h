#pragma once

#include "bytecode.h"
#include <unordered_set>

using Result = std::unordered_set<uintmax_t>;
using Resptr = std::unique_ptr<Result>;

class Engine {
  public:
	// returns whether there is atleast one 1 in the sequence
	static bool   next_sequence(bool *sq, int num);
	static void print_sequence(bool *sq, int num);
	static int    solve(Bytecode &code);
	static bool execute(const unsigned char *ins, bool *stack_, int slots);
};
