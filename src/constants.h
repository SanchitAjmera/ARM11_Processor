#ifndef CONSTANTS_H
#define CONSTANTS_H

// general constants
#define MEMORY_CAPACITY 65536
#define NO_REGISTERS 17
#define ADDRESS_SIZE 4
#define CPSR_N_MASK (1 << 31)
#define CPSR_Z_MASK (1 << 30)
#define CPSR_C_MASK (1 << 29)
#define CPSR_V_MASK (1 << 28)
#define CPSR_N_SHIFT 31
#define CPSR_Z_SHIFT 30
#define CPSR_C_SHIFT 29
#define CPSR_V_SHIFT 28
#define GET_CPSR_N(cpsr) ((cpsr & CPSR_N_MASK) >> CPSR_N_SHIFT)
#define GET_CPSR_Z(cpsr) ((cpsr & CPSR_Z_MASK) >> CPSR_Z_SHIFT)
#define GET_CPSR_C(cpsr) ((cpsr & CPSR_C_MASK) >> CPSR_C_SHIFT)
#define GET_CPSR_V(cpsr) ((cpsr & CPSR_V_MASK) >> CPSR_V_SHIFT)
#define GET_CPSR_FLAGS(cpsr) (cpsr >> 28)
#define BITS_SET(value, mask, bits) ((value & mask) == bits)

// constants for decode function
#define DECODE_DPI_MASK 0x0C000000
#define DECODE_DPI_EXPECTED 0x00000000
#define DECODE_MULT_MASK 0x0FC000F0
#define DECODE_MULT_EXPECTED 0x0000090
#define DECODE_SDT_MASK 0x0C600000
#define DECODE_SDT_EXPECTED 0x04000000
#define DECODE_BRANCH_MASK 0x0F000000
#define DECODE_BRANCH_EXPECTED 0x0A000000

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

// constants for Branch instruction
#define BRANCH_OFFSET_MASK 0x00FFFFFF
#define BRANCH_SIGN_BIT (1 << 23)
#define CURRENT_INSTRUCTION_SHIFT 2
#define NEGATIVE_SIGN_EXTEND 0xFC000000
#define POSITIVE_SIGN_EXTEND 0

#endif
