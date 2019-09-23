#include "optimizer.h"
#include <iomanip>

#define CASE(x) case BytecodeHolder::CODE_##x
#define int_at(x) (*(int *)(InstructionPointer + (x)))
#define next_int() ((i += sizeof(int)))

using namespace std;

void Optimizer::optimize(Bytecode &orig) {
	const unsigned char *InstructionPointer = orig->bytecodes.data();
	for(int i = 0; i < orig->getip();) {
		switch(InstructionPointer[i]) {
			CASE(load_slot) : {
				i++;
				next_int();
				break;
			}
			CASE(jumpiffalse) : {
				int loc     = int_at(i + 1);
				int origloc = loc;
				while(InstructionPointer[i + loc] ==
				      BytecodeHolder::CODE_jumpiffalse) {
					loc += int_at(i + loc + 1);
				}
				if(InstructionPointer[i + loc] ==
				   BytecodeHolder::CODE_jumpiftrue) {
					loc += 1 + sizeof(int);
				}
				if(loc != origloc) {
					orig->jumpiffalse(i, loc);
#ifdef DEBUG
					cout << setw(4) << i << ": " << setw(0) << "jumpiffalse "
					     << origloc << " -> " << loc << "\n";
#endif
				}
				i++;
				next_int();
				break;
			}
			CASE(load_slot_0)
			    : CASE(load_slot_1)
			    : CASE(load_slot_2)
			    : CASE(load_slot_3)
			    : CASE(load_slot_4)
			    : CASE(load_slot_5)
			    : CASE(load_slot_6)
			    : CASE(load_slot_7)
			    : CASE(halt) : CASE(and_) : CASE(or_) : CASE(complement) : {
				i++;
				break;
			}
			CASE(jumpiftrue) : {
				int loc     = int_at(i + 1);
				int origloc = loc;
				while(InstructionPointer[i + loc] ==
				      BytecodeHolder::CODE_jumpiftrue) {
					loc += int_at(i + loc + 1);
				}
				if(InstructionPointer[i + loc] ==
				   BytecodeHolder::CODE_jumpiffalse) {
					loc += 1 + sizeof(int);
				}
				if(loc != origloc) {
					orig->jumpiftrue(i, loc);
#ifdef DEBUG
					cout << setw(4) << i << ": " << setw(0) << "jumpiftrue "
					     << origloc << " -> " << loc << "\n";
#endif
				}
				i++;
				next_int();
				break;
			}
		}
	}
}
