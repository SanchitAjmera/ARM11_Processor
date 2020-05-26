#ifndef EMULATE_UTIL_H
#define EMULATE_UTIL_H

#include <stdint.h>
#include <stdlib.h>

typedef uint32_t Word;
typedef uint8_t Byte;

static const int MEM_BYTE_CAPACITY	= 65536;
static const int WORD_LEN						= 4;
static const int REG_COUNT					= 17;

struct arm {
	Byte* memory;

	/* 0-12 general purpose, 13 SP, 14 LR, 15 PC, 16 CPSR */
	Word* registers;
};

extern void check_ptr(const void* ptr, const char* error_msg);


/* Takes in the ARM binary file's name and returns an ARM state pointer with memory and register
 * pointers on heap, where memory is of size MEM_LIMIT bytes */
extern void init_arm(struct arm* state, const char* fname);

#endif
