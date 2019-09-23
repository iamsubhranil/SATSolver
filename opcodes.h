// Stack effect indicates the total change of the
// stack after the operation

// X in OPCODEX signifies X bytecode arguments

#ifndef OPCODE0
#define OPCODE0(name, stackEffect)
#endif

#ifndef OPCODE1
#define OPCODE1(name, stackEffect, type1)
#endif

#ifndef OPCODE2
#define OPCODE2(name, stackEffect, type1, type2)
#endif

#ifndef OPCODE3
#define OPCODE3(name, stackEffect, type1, type2, type3)
#endif

OPCODE0(and_, -1)
OPCODE0(or_, -1)
OPCODE0(complement, 0)

OPCODE0(load_slot_0, 1)
OPCODE0(load_slot_1, 1)
OPCODE0(load_slot_2, 1)
OPCODE0(load_slot_3, 1)
OPCODE0(load_slot_4, 1)
OPCODE0(load_slot_5, 1)
OPCODE0(load_slot_6, 1)
OPCODE0(load_slot_7, 1)
OPCODE1(load_slot, 1, int)

// Used for short circuit.
// If found true or false for respective operators,
// it leaves the result on the stack and doesn't compute
// the right operand.
OPCODE1(jumpiftrue, 0, int)
OPCODE1(jumpiffalse, 0, int)

OPCODE0(halt, 0)

#undef OPCODE0
#undef OPCODE1
#undef OPCODE2
