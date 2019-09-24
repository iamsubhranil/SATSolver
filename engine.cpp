#include "engine.h"
#include "bytecode.h"
#include "display.h"
#include <string.h>
#include <unordered_set>

using namespace std;

bool Engine::next_sequence(bool *sq, int num) {
	--num;
	bool ret = false;
	while(num >= 0 && sq[num]) {
		sq[num--] = false;
	}
	if(num >= 0) {
		sq[num] = true;
		ret     = true;
	}
	return ret;
}

void Engine::print_sequence(bool *sq, int num) {
	for(int i = 0; i < num; i++) {
		cout << (sq[i] ? "1" : "0");
	}
}

int Engine::solve(Bytecode &b) {
	bool *               s   = (bool *)malloc(sizeof(bool) * b->maxStackSize());
	const unsigned char *ins = b->bytecodes.data();
	memset(s, 0, sizeof(bool) * b->maxStackSize());
	int       slots     = b->numSlots();
	uintmax_t satisfied = 0;
	// check for 0...0
	if(execute(ins, s, slots)) {
		print_sequence(s, slots);
		cout << " : satisfied\n";
		satisfied++;
		// satisfied->insert(i + 1); // next_sequence starts from 00..0001
	}
	while(next_sequence(s, slots)) {
		if(execute(ins, s, slots)) {
			print_sequence(s, slots);
			cout << " : satisfied\n";
			satisfied++;
			// satisfied->insert(i + 1); // next_sequence starts from 00..0001
		}
	}
	return satisfied;
}

bool Engine::execute(const unsigned char *ins, bool *Stack, int slots) {

#define next_int()                      \
	(InstructionPointer += sizeof(int), \
	 *(int *)((InstructionPointer - sizeof(int) + 1)))

#define skip_int() (InstructionPointer += sizeof(int))

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
				if(TOP) {
					int to = next_int();
					JUMPTO(to - sizeof(int));
				}
				skip_int();
				DISPATCH();
			}

			CASE(jumpiffalse) : {
				if(!TOP) {
					int to = next_int();
					JUMPTO(to - sizeof(int));
				}
				skip_int();
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
