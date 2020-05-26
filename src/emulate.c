#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_CAPACITY 65536
#define NO_REGISTERS 17
#define ADDRESS_SIZE 4
#define BITS_SET(value, mask, bits) ((value & mask) == bits)

typedef uint32_t word;

/*registers 0-12 will be used by their value so for reg0 we can just use 0
but these will make it easier to address in memory*/
enum Register {PC = 15, CPSR = 16};
// opcode mnemonics
enum Opcode {AND, EOR, SUB, RSB, ADD, TST = 8, TEQ, CMP, ORR = 12, MOV};
// condition suffixes
enum Cond {EQ, NE, GE = 10, LT, GT, LE, AL};

typedef struct {
  // ARM machine memory
  uint8_t * memory;
  // 0-12 general purpose, 13 SP, 14 LR, 15 PC, 16 CPSR
  word * registers;
} arm;

void ptrValidate(const void * pointer, char * error) {
  if (pointer == NULL) {
    printf("Error: %s\n", error);
    exit(EXIT_FAILURE);
  }
}

bool checkCond(arm state, word instruction) {
  // CPSR flag bits
  unsigned int n = state.registers[CPSR] & 0x80000000;
  unsigned int z = state.registers[CPSR] & 0x40000000;
  unsigned int c = state.registers[CPSR] & 0x20000000;
  unsigned int v = state.registers[CPSR] & 0x10000000;
  unsigned int cond = instruction >> 28;
  // conditions for instruction
  switch (cond) {
    case EQ:
      return z;
    case NE:
      return !z;
    case GE:
      return n == v;
    case LT:
      return n != v;
    case GT:
      return !z && (n == v);
    case LE:
      return z || (n != v);
    case AL:
      return true;
    default:
      return false;
  }
}

void dpi(arm state, word instruction) {
  if (!checkCond(state, instruction)) {
    return;
  }
  // parts of the instruction
  unsigned int i = instruction & 0x02000000;
  unsigned int s = instruction & 0x00100000;
  unsigned int rn = instruction & 0x000F0000;
  unsigned int rd = instruction & 0x0000F000;
  unsigned int op2 = instruction & 0x00000FFF;

  // TODO:
    // If the S bit is 0, the CPSR register is unaffected
    // If the S bit is set then the CPSR flags should be set as follows:
      // The V bit will be unaffected.
      // The C bit in logical operations will be set to the carry out from any shift operation
        // In arithmetic operations the C bit will be set to the carry out of the bit 31 of the ALU
        // C is set to 1 if the addition produced a carry
        // For subtraction (including comparison), the bit C is set to 0 if the subtraction produced a borrow
      // The Z bit will be set only if the result is all zeros.
      // The N bit will be set to the logical value of bit 31 of the result.

  // TODO: operand2 is an immediate value
  // TODO: operand2 is a register

  // execution of instruction
  enum Opcode opcode = instruction & 0x01E00000;
  switch (opcode) {
    case AND:
      // Rn AND operand2
      break;
    case EOR:
      // Rn EOR operand2
      break;
    case SUB:
      // Rn - operand2
      break;
    case RSB:
      // operand2 - Rn
      break;
    case ADD:
      // Rn + operand2
      break;
    case TST:
      // as and, but result not written
      break;
    case TEQ:
      // as eor, but result is not written
      break;
    case CMP:
      // as sub, but result is not written
      break;
    case ORR:
      // Rn OR operand2
      break;
    case MOV:
      // operand2 (Rn is ignored)
      break;
  }
}

void decode(arm state, word instruction) {
  const word dpMask = 0x0C000000;
  const word dp = 0x00000000;
  const word multMask = 0x0FC000F0;
  const word mult = 0x0000090;
  const word sdtMask = 0x0C600000;
  const word sdt = 0x04000000;
  const word branchMask = 0x0F000000;
  const word branch = 0x0A000000;

  // TODO: determine how to differentiate ...
  // ... `data processing` from `multiply`

  if (BITS_SET(instruction, branchMask, branch)) {
    // function for branch instructions
  } else if (BITS_SET(instruction, sdtMask, sdt)) {
    // function for single data tranfser instructions
  } else if (BITS_SET(instruction, multMask, mult)) {
    // function for multiply instructions
  } else if (BITS_SET(instruction, dpMask, dp)) {
    dpi(state, instruction);
  }
}

int main (int argc, char ** argv) {
  if (argc == 1) {
    printf("Please specify an ARM binary object code file.\n");
    exit(EXIT_FAILURE);
  }
  arm * state;

  // free memory before code termination
  free(state -> memory);
  free(state -> registers);
  free(state);
  return EXIT_SUCCESS;

}
