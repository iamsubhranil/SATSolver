#include "engine.h"
#include "bytecode.h"
#include "display.h"
#include <string.h>
#include <unordered_set>

using namespace std;

void Engine::next_sequence(bool *sq, int num) {
	if(sq[num - 1] == false) {
		sq[num - 1] = true;
	} else {
		int n = num - 1;
		while(sq[n]) {
			sq[n--] = false;
		}
		sq[n] = true;
	}
}

void Engine::print_sequence(bool *sq, int num) {
	for(int i = 0; i < num; i++) {
		cout << (sq[i] ? "1" : "0");
	}
}

void Engine::solve(Bytecode &b) {
	bool *               s   = (bool *)malloc(sizeof(bool) * b->maxStackSize());
	const unsigned char *ins = b->bytecodes.data();
	memset(s, 0, sizeof(bool) * b->maxStackSize());
	int       slots = b->numSlots();
	uintmax_t loop  = 1 << slots;
	// unordered_set<uintmax_t> satisfied;
	for(uintmax_t i = 0; i < loop; i++) {
		next_sequence(s, slots);
		execute(ins, s, slots);
		// print_sequence(s, slots);
		// cout << " : ";
		// cout << "satisfied\n";
	}
}

bool Engine::execute(const unsigned char *ins, bool *Stack, int slots) {

#define next_int()                      \
	(InstructionPointer += sizeof(int), \
	 *(int *)((InstructionPointer - sizeof(int) + 1)))

#ifdef SAT_USE_COMPUTED_GOTO
#define DEFAULT() EXEC_CODE_unknown
	static const void *dispatchTable[] = {
#define OPCODE0(x, y) &&EXEC_CODE_##x,
#define OPCODE1(x, y, z) OPCODE0(x, y)
#define OPCODE2(w, x, y, z) OPCODE0(w, x)
#include "opcodes.h"
	    &&DEFAULT()};

#define LOOP() while(1)
#define SWITCH() \
	{ goto *dispatchTable[*InstructionPointer]; }
#define CASE(x) EXEC_CODE_##x
#define DISPATCH() \
	{ goto *dispatchTable[*(++InstructionPointer)]; }
#define DISPATCH_WINC() \
	{ goto *dispatchTable[*InstructionPointer]; }
#else
#define LOOP() while(1)
#define SWITCH() switch(*InstructionPointer)
#define CASE(x) case BytecodeHolder::CODE_##x
#define DISPATCH()            \
	{                         \
		InstructionPointer++; \
		continue;             \
	}
#define DISPATCH_WINC() \
	{ continue; }
#define DEFAULT() default
#endif

#define TOP (*(StackTop - 1))
#define POP() (*(--StackTop))

#define JUMPTO(x)                                      \
	{                                                  \
		InstructionPointer = InstructionPointer + (x); \
		continue;                                      \
	}
#define PUSH(x) *StackTop++ = (x);

#define LOAD_SLOT(x)        \
	CASE(load_slot_##x) : { \
		PUSH(Stack[x]);     \
		DISPATCH();         \
	}

	const unsigned char *InstructionPointer = ins;
	bool *               StackTop           = Stack + slots;

	LOOP() {

		SWITCH() {

			CASE(load_slot) : {
				PUSH(Stack[next_int()]);
				DISPATCH();
			}

			LOAD_SLOT(0)
			LOAD_SLOT(1)
			LOAD_SLOT(2)
			LOAD_SLOT(3)
			LOAD_SLOT(4)
			LOAD_SLOT(5)
			LOAD_SLOT(6)
			LOAD_SLOT(7)

			CASE(and_) : {
				bool b = POP();
				TOP &= b;
				DISPATCH();
			}

			CASE(or_) : {
				bool b = POP();
				TOP |= b;
				DISPATCH();
			}

			CASE(complement) : {
				TOP = !TOP;
				DISPATCH();
			}

			CASE(jumpiftrue) : {
				int to = next_int();
				if(TOP) {
					JUMPTO(to - sizeof(int));
				}
				DISPATCH();
			}

			CASE(jumpiffalse) : {
				int to = next_int();
				if(TOP) {
					JUMPTO(to - sizeof(int));
				}
				DISPATCH();
			}

			CASE(halt) : { return TOP; }

			DEFAULT() : {
				uint8_t code = *InstructionPointer;
				if(code > BytecodeHolder::CODE_halt) {
					panic("Invalid bytecode %d!", code);
				} else {
					panic("Bytecode not implemented : '%s'!",
					      BytecodeHolder::OpcodeNames[code])
				}
			}
		}
	}

	return TOP;
}
