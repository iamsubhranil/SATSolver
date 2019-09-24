#pragma once

#include "bytecode.h"
#include <unordered_set>

using Result = std::unordered_set<uintmax_t>;
using Resptr = std::unique_ptr<Result>;

class Engine {
  public:
	static void next_sequence(bool *sq, int num);
	static void print_sequence(bool *sq, int num);
	static Resptr solve(Bytecode &code);
	static bool execute(const unsigned char *ins, bool *stack_, int slots);
};
