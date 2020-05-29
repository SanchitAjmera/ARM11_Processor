#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulate_util.h"
#include <assert.h>

// need to delete before merge --------------------------------------
typedef uint32_t word;
typedef uint8_t byte;
typedef unsigned int uint;

static const int MEM_BYTE_CAPACITY = 65536;
static const int WORD_LEN = 4;
static const int REG_COUNT = 17;

// general constants
#define MEMORY_CAPACITY 65536
#define NO_REGISTERS 17
#define ADDRESS_SIZE 4
#define WORD_SIZE 32
#define MSB_MASK 1 << 31
#define LSB_MASK 0x1
#define BITS_SET(value, mask, bits) ((value & mask) == bits)

// constants for Multiply instruction
#define MULT_RDEST_MASK 0x000F0000
#define MULT_REG_S_MASK 0x00000F00
#define MULT_REG_M_MASK 0x0000000F
#define MULT_REG_N_MASK 0x0000F000
#define MULT_REG_S_SHIFT 8
#define MULT_REG_N_SHIFT 12
#define MULT_RDEST_SHIFT 16
#define ACCUMULATE_FLAG (1 << 21)
#define UPDATE_CPSR (1 << 20)
#define CPSR_N (1 << 31)
#define CPSR_Z (1 << 30)

// constants for Branch instruction
#define BRANCH_OFFSET_MASK 0x00FFFFFF
#define BRANCH_SIGN_BIT (1 << 23)
#define CURRENT_INSTRUCTION_SHIFT 2
#define NEGATIVE_SIGN_EXTEND 0xFC000000
#define POSITIVE_SIGN_EXTEND 0

// constants for Data Processing instruction
#define TWOS_COMPLEMENT(value) ((~value) + 1)
#define CPSR_V_MASK 0x10000000
#define SET_CPSR_C (1 << 29)
#define DPI_I_MASK 0x02000000
#define DPI_OPCODE_MASK 0x01E00000
#define DPI_S_MASK 0x00100000
#define DPI_RN_MASK 0xF0000
#define DPI_RD_MASK 0xF000
#define DPI_OP2_MASK 0x00000FFF
#define DPI_I_SHIFT 25
#define DPI_OPCODE_SHIFT 21
#define DPI_S_SHIFT 20
#define DPI_RN_SHIFT 16
#define DPI_RD_SHIFT 12
#define LEAST_BYTE 0xFF
#define GET_ROTATE_SHIFT 8
#define ROTATION_FACTOR 2
#define GET_RS_SHIFT 4
#define SHIFT_TYPE_MASK 0x06
#define GET_SHIFT_TYPE_SHIFT 1
#define LSN_MASK 0xF
#define GET_SHIFT_INSTRUCTION_SHIFT 4
#define GET_SHIFT_CONSTANT_SHIFT 3

// Main assumptions about the emulator/ ARM
#define MEMORY_CAPACITY 65536
#define NO_REGISTERS 17
#define ADDRESS_SIZE 4
#define BITS_SET(value, mask, bits) ((value & mask) == bits)

// delete all above before submitting -----------------
// constants for masks
// Constants for single data instruction
#define SDTI_I_MASK 0x02000000
#define SDTI_P_MASK 0x01000000
#define SDTI_U_MASK 0x00800000
#define SDTI_L_MASK 0x00100000
#define SDTI_RN_MASK 0x000F0000
#define SDTI_RD_MASK 0x0000F000
#define SDTI_OFFSET_MASK 0x00000FFF
#define SDTI_I_SHIFT 25
#define SDTI_P_SHIFT 24
#define SDTI_U_SHIFT 23
#define SDTI_L_SHIFT 20
#define SDTI_RN_SHIFT 16
#define SDTI_RD_SHIFT 12

// an unassigned 32 bit int for a word

typedef uint32_t word;

enum Register { PC = 15, CPSR = 16 };

// condition suffixes for their codes
enum Cond { EQ, NE, GE = 10, LT, GT, LE, AL };

typedef struct {
  // ARM machine memory
  uint8_t *memory;
  // 0-12 general purpose , 13 SP, 14 LR, 15 PCM 16 CPSR
  word *registers;
} arm;

// CHECK IF CORRECTLY ASSIGNED MEMORY
void ptrValidate(const void *pointer, char *error) {
  if (pointer == NULL) {
    printf("Error: %s\n", error);
    exit(EXIT_FAILURE);
  }
}

// function to check conditions
// Parameter 1: code for Cond
// Parameter 2: current state of arm
bool checkCond(word instruction, arm *state) {
  // CPSR FLAG BITS - WE USE 1,2,4,8 to extract the 1,2,3,4th bit in the cpsr
  // address
  unsigned int n = (state->registers[CPSR] & 0x80000000) >> 31;
  unsigned int z = (state->registers[CPSR] & 0x40000000) >> 30;
  unsigned int c = (state->registers[CPSR] & 0x20000000) >> 29;
  unsigned int v = (state->registers[CPSR] & 0x10000000) >> 28;
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
    ;
  case AL:
    return true;
  default:
    return false;
  }
}

// -Alex's DPI ---------------------------------------------------------------
typedef unsigned int uint;

void check_ptr(const void *ptr, const char *error_msg) {
  if (ptr == NULL) {
    printf("Error: %s\n", error_msg);
    exit(EXIT_FAILURE);
  }
}

// TODO: consider changing name (and type of carryOut)
typedef struct {
  word result;
  word carryOut;
} tuple_t;

/*registers 0-12 will be used by their value so for reg0 we can just use 0
but these will make it easier to address in memory*/

// opcode mnemonics
enum Opcode { AND, EOR, SUB, RSB, ADD, TST = 8, TEQ, CMP, ORR = 12, MOV };
// condition suffixes

// shift types
enum Shift { LSL, LSR, ASR, ROR };

// dpi ----------------------------------------------------------------------

word rotateRight(word value, uint rotateNum) {
  uint lsbs = value & ((1 << rotateNum) - 1);
  return (value >> rotateNum) | (lsbs << (WORD_SIZE - rotateNum));
}

word arithShift(word value, uint shiftNum) {
  word msb = value & MSB_MASK;
  // TODO: change variable name
  word msbs = msb;
  for (int i = 0; i < shiftNum; i++) {
    msbs = msbs >> 1;
    msbs = msbs + msb;
  }
  return msbs | (value >> shiftNum);
}

uint shiftByConstant(uint shiftPart) {
  // integer specified by bits 7-4
  return shiftPart >> GET_SHIFT_CONSTANT_SHIFT;
}

uint shiftByRegister(arm *state, uint shiftPart) {
  // Rs (register) can be any general purpose register except the PC
  uint rs = shiftPart >> GET_RS_SHIFT;
  // bottom byte of value in Rs specifies the amount to be shifted
  return state->registers[rs] & LEAST_BYTE;
}

uint leftCarryOut(word value, uint shiftNum) {
  return (value << (shiftNum - 1)) >> (WORD_SIZE - 1);
}

uint rightCarryOut(word value, uint shiftNum) {
  return (value >> (shiftNum - 1)) & LSB_MASK;
}

tuple_t *barrelShifter(arm *state, word value, uint shiftPart) {
  // bit to determine what to shift by
  bool shiftByReg = shiftPart & LSB_MASK;
  // number to shift by
  uint shiftNum = shiftByReg ? shiftByRegister(state, shiftPart)
                             : shiftByConstant(shiftPart);
  // bits that specify the shift operation
  enum Shift shiftType = (shiftPart & SHIFT_TYPE_MASK) >> GET_SHIFT_TYPE_SHIFT;
  // tuple for the result and the carry out bit
  tuple_t *output = (tuple_t *)malloc(sizeof(tuple_t));
  check_ptr(output, "Not enough memory!");
  word result;
  word carryOut = rightCarryOut(value, shiftNum);
  switch (shiftType) {
  case LSL:
    carryOut = leftCarryOut(value, shiftNum);
    result = value << shiftNum;
    break;
  case LSR:
    result = value >> shiftNum;
    break;
  case ASR:
    result = arithShift(value, shiftNum);
    break;
  case ROR:
    result = rotateRight(value, shiftNum);
    break;
  default:
    // no other shift instruction
    // should never happen
    assert(false);
  }
  output->result = result;
  output->carryOut = carryOut;
  return output;
}

tuple_t *opRegister(arm *state, uint op2) {
  // register that holds the value to be shifted
  uint rm = op2 & LSN_MASK;
  // value to be shifted
  word value = state->registers[rm];
  // bits indicating the shift instruction (bits 11-4)
  uint shiftPart = op2 >> GET_SHIFT_INSTRUCTION_SHIFT;
  return barrelShifter(state, value, shiftPart);
}

tuple_t *opImmediate(arm *state, uint op2) {
  // 8-bit immediate value zero-extended to 32 bits
  word imm = op2 & LEAST_BYTE;
  // number to rotate by
  uint rotateNum = (op2 >> GET_ROTATE_SHIFT) * ROTATION_FACTOR;
  // tuple for the result and the carry out bit
  tuple_t *output = (tuple_t *)malloc(sizeof(tuple_t));
  check_ptr(output, "Not enough memory!");
  // result of the rotation operation
  output->result = rotateRight(imm, rotateNum);
  // carry out for CSPR flag
  output->carryOut = rightCarryOut(imm, rotateNum);
  return output;
}

word getCarryOut(word op1, word op2, word result) {
  uint carryOut = 0;
  word signBit = MSB_MASK;
  // overflow occurs iff the operands have the same sign and the result has ...
  // ... the opposite sign
  if ((op1 & signBit) == (op2 & signBit)) {
    carryOut = (op1 & signBit) != (result & signBit);
  }
  return carryOut;
}

void setCPSR(arm *state, word result, word carryOut) {
  // set to the logical value of bit 31 of the result
  word n = result & CPSR_N;
  // set only if the result is all zeros
  word z = result ? 0 : CPSR_Z;
  // carry out from the instruction
  word c = carryOut ? SET_CPSR_C : 0;
  // v is unaffected
  word v = state->registers[CPSR] & CPSR_V_MASK;
  // updated flag bits
  state->registers[CPSR] = n | z | c | v;
}

//-Single Data Tranfer Instructions function ----------------------------------

void sdti(arm *state, word instruction) {
  if (!checkCond(instruction & 0xF0000000, state)) {
    printf("cond failed");
    return;
  }
  // parts of the instruction
  unsigned int i = (instruction & SDTI_I_MASK) >> SDTI_I_SHIFT;
  unsigned int p = (instruction & SDTI_P_MASK) >> SDTI_P_SHIFT;
  unsigned int u = (instruction & SDTI_U_MASK) >> SDTI_U_SHIFT;
  unsigned int l = (instruction & SDTI_L_MASK) >> SDTI_L_SHIFT;
  unsigned int rn = (instruction & SDTI_RN_MASK) >> SDTI_RN_SHIFT;
  unsigned int rd = (instruction & SDTI_RD_MASK) >> SDTI_RD_SHIFT;
  word offset = (instruction & SDTI_OFFSET_MASK);

  // Immediate Offset
  tuple_t *output = i ? opRegister(state, offset) : opImmediate(state, offset);
  offset = output->result;

  // see if i need to load or store Data

  if (l) {
  }

  // p doesn't change contents of base register for this exercise
  if (p) {
    if (u) {
      // offset is added to base register if u is set
      rn += offset;

    } else {
      // subtract offset from base register
      rn -= offset;
    }

    if (l) {
      // check if address is out of bounds (MAX == 64KB)
      // laod data from address from rn into rd
      word ldr = state->registers[rn];
      state->registers[rd] = ldr;
    } else {
      word str = state->registers[rd];
      state->registers[rn] = str;
    }

  } else { // if post=indexing

    if (l) {
      // check if address is out of bounds (MAX == 64KB)
      // laod data from address from rn into rd
      word ldr = state->registers[rn];
      state->registers[rd] = ldr;
    } else {
      word str = state->registers[rd];
      state->registers[rn] = str;
    }

    if (u) {
      // offset is added to base register if u is set
      rn += offset;

    } else {
      // subtract offset from base register
      rn -= offset;
    }
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
    // function for data processing instructions
  }
}

void printBits(uint32_t x) {
  uint32_t mask = 1 << 31;
  for (int i = 0; i < 32; i++) {
    printf("%i", (x & mask) != 0);
    x <<= 1;
  }

  printf("\n");
}

int main(int argc, char **argv) {
  arm state;
  state.memory = (byte *)calloc(65536, sizeof(byte));
  state.registers = (word *)calloc(17, sizeof(word));

  // STRUCTURE
  // COND - 01 - I - P - U - 00 - L - Rn - Rd - Offset
  // 1110 01 1 0 1 00 1 - 0000 - 0100 - 000000000000
  word instr = 0b11100110100100000100000000000000;

  state.registers[0] = 0xFDCE0873;
  state.registers[1] = 0x0000000F;
  state.registers[2] = 0X00000001;
  state.registers[CPSR] = 0x90000000;
  // ACTUAL ANSWER
  state.registers[4] = 0xFDCE0873 & (0x0000000F << 3);
  printf("state before: \n");
  for (int i = 0; i < 5; i++) {
    printf("Register %d: ", i);
    printBits(state.registers[i]);
  }
  printf("\n");

  sdti(&state, instr);
  printf("state after: \n");
  for (int i = 0; i < 5; i++) {
    printf("Register %d: ", i);
    printBits(state.registers[i]);
  }

  return 0;
}

/*
int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Please specify an ARM binary object code file.\n");
    exit(EXIT_FAILURE);
  }
  arm *state;

  // free memory before code termination
  free(state->memory);
  free(state->registers);
  free(state);
  return EXIT_SUCCESS;
}
*/
