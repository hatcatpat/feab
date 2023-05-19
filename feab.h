#include <stdio.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define SQR(x) ((x) * (x))

#define MAX_PROGRAM_SIZE 256
#define MAX_OPCODE_LENGTH (4 + 1)
#define NUM_SPRITES 16
#define STACK_SIZE 16

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int int16_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long int int64_t;
typedef unsigned long int uint64_t;
typedef unsigned int uint_t;

typedef enum { false, true } bool;

void
assemble(char *file);

void
load(char *file);

enum opcodes
{
    OP_QUIT,
    OP_WAIT,
    OP_RET,
    OP_JUMP,
    OP_CALL,
    OP_IF,
    OP_ELSE,
    OP_LESS,
    OP_MORE,
    OP_SET,
    OP_INC,
    OP_DEC,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_MOD,
    OP_CMP,
    NUM_OPCODES
};

typedef struct
{
    char string[MAX_OPCODE_LENGTH];
    uint8_t args;
} opcode_t;

extern opcode_t opcode[NUM_OPCODES];

enum modes
{
    MODE_VALUE_VALUE,
    MODE_VALUE_ADDR,
    MODE_ADDR_VALUE,
    MODE_ADDR_ADDR
};

enum flags
{
    FLAG_QUIT = (1 << 0),
    FLAG_WAIT = (1 << 1),
    FLAG_LOAD_SPRITE = (1 << 2),
    FLAG_EQUAL = (1 << 3),
    FLAG_LESS = (1 << 4),
    FLAG_MORE = (1 << 5),
    FLAG_OVERFLOW = (1 << 6),
    FLAG_UNDERFLOW = (1 << 7)
};

enum keys
{
    KEY_LEFT = (1 << 0),
    KEY_RIGHT = (1 << 1),
    KEY_UP = (1 << 2),
    KEY_DOWN = (1 << 3),
    KEY_Z = (1 << 4),
    KEY_X = (1 << 5),
    KEY_C = (1 << 6),
    KEY_V = (1 << 7)
};

#define SPRITE_DATA(x)\
    MEMORY_SPRITE_##x##_X,\
	MEMORY_SPRITE_##x##_Y,\
    MEMORY_SPRITE_##x##_ROW_0,\
    MEMORY_SPRITE_##x##_ROW_1,\
    MEMORY_SPRITE_##x##_ROW_2,\
    MEMORY_SPRITE_##x##_ROW_3

enum memory
{
    MEMORY_FLAGS,
    MEMORY_KEYS,
    MEMORY_PALETTE_0, MEMORY_PALETTE_1, MEMORY_PALETTE_2, MEMORY_PALETTE_3,
    MEMORY_SPRITES_ROW_0_FLAGS, MEMORY_SPRITES_ROW_1_FLAGS,
    SPRITE_DATA(0), SPRITE_DATA(1), SPRITE_DATA(2), SPRITE_DATA(3),
    SPRITE_DATA(4), SPRITE_DATA(5), SPRITE_DATA(6), SPRITE_DATA(7),
    SPRITE_DATA(8), SPRITE_DATA(9), SPRITE_DATA(10), SPRITE_DATA(11),
    SPRITE_DATA(12), SPRITE_DATA(13), SPRITE_DATA(14), SPRITE_DATA(15),
    PROGRAM_START
};

#undef SPRITE_DATA

typedef struct
{
    uint8_t memory[PROGRAM_START + MAX_PROGRAM_SIZE];
    uint_t length;
    uint16_t stack[STACK_SIZE], sc, pc;
} feab_t;

extern feab_t feab;

void init();
void run();
void load(char *file);
void print_memory();
void print_flags();
