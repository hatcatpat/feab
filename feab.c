#include "feab.h"

void
set(void *data, size_t size, int value)
{
    uint8_t *p = data;
    while(size--)
        *p++ = value;
}

opcode_t opcode[NUM_OPCODES] =
{
    { "QUIT", 0 },
    { "WAIT", 0 },
    { "RET", 0 },
    { "JUMP", 1 },
    { "JUMPV", 1 },
    { "CALL", 1 },
    { "CALLV", 1 },
    { "IF", 1 },
    { "IFV", 1 },
    { "ELSE", 1 },
    { "ELSEV", 1 },
    { "LESS", 1 },
    { "LESSV", 1 },
    { "MORE", 1 },
    { "MOREV", 1 },
    { "SET", 2 },
    { "SETV", 2 },
    { "DEREF", 2 },
    { "DEREFV", 2 },
    { "REF", 2 },
    { "REFV", 2 },
    { "ADD", 2 },
    { "SUB", 2 },
    { "INC", 1 },
    { "DEC", 1 },
    { "AND", 2 },
    { "OR", 2 },
    { "NOT", 1 },
    { "MOD", 2 },
    { "CMP", 2 },
    { "CHAR", 1 },
};

feab_t feab;

void
load(const char *file)
{
    uint_t length;
    int i;
    FILE *f = fopen(file, "r");

    init();

    if(f == NULL)
        return;

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(length > MAX_PROGRAM_SIZE)
        {
            fprintf(stderr, "[error] program too large! %i\n", length);
            goto cleanup;
        }

    feab.length = PROGRAM_START + length;
    for(i = 0; i < length; ++i)
        feab.memory[PROGRAM_START + i] = fgetc(f);

    printf("[loaded %s] ", file);
    for(i = 0; i < length; ++i)
        printf("%i ", feab.memory[PROGRAM_START + i]);
    printf("\n");

cleanup:
    fclose(f);
}

void
init()
{
    set(&feab, sizeof(feab_t), 0);
    feab.pc = PROGRAM_START;
}

void
run()
{
    uint8_t current, code, mode;
    uint16_t a, b;

    feab.memory[MEMORY_FLAGS] &= ~FLAG_WAIT;

    while(feab.pc < feab.length && !(feab.memory[MEMORY_FLAGS] & (FLAG_QUIT | FLAG_WAIT)))
        {
            current = feab.memory[feab.pc++];
            code = current & ((1 << 6) - 1);
            mode = current >> 6;

#ifdef DEBUG
            printf("[pc %i] opcode: %i %s\n", feab.pc - 1, code, opcode[code].string);
#endif

            if(opcode[code].args > 0)
                {
                    switch(mode)
                        {
                            case MODE_VALUE_VALUE:
                                a = feab.memory[feab.pc++];
                                if(opcode[code].args == 2)
                                    b = feab.memory[feab.pc++];
                                break;

                            case MODE_VALUE_ADDR:
                                a = feab.memory[feab.pc++];
                                if(opcode[code].args == 2)
                                    b = (feab.memory[feab.pc++] << 8), b |= (feab.memory[feab.pc++]);
                                break;

                            case MODE_ADDR_VALUE:
                                a = (feab.memory[feab.pc++] << 8), a |= (feab.memory[feab.pc++]);
                                if(opcode[code].args == 2)
                                    b = feab.memory[feab.pc++];
                                break;

                            case MODE_ADDR_ADDR:
                                a = (feab.memory[feab.pc++] << 8), a |= (feab.memory[feab.pc++]);
                                if(opcode[code].args == 2)
                                    b = (feab.memory[feab.pc++] << 8), b |= (feab.memory[feab.pc++]);
                                break;
                        }
                }

            switch(code)
                {
                    case OP_QUIT:
                        feab.memory[MEMORY_FLAGS] |= FLAG_QUIT;
                        break;

                    case OP_WAIT:
                        feab.memory[MEMORY_FLAGS] |= FLAG_WAIT;
                        break;

                    case OP_RET:
                        feab.pc = feab.stack[--feab.sc];
                        break;

                    case OP_JUMPV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_JUMP:
                        feab.pc = a;
                        break;

                    case OP_CALLV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_CALL:
                        feab.stack[feab.sc++] = feab.pc;
                        feab.pc = a;
                        break;

                    case OP_IFV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_IF:
                        if(feab.memory[MEMORY_FLAGS] & FLAG_EQUAL)
                            feab.pc = a;
                        break;

                    case OP_ELSEV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_ELSE:
                        if(feab.memory[MEMORY_FLAGS] & (FLAG_LESS | FLAG_MORE))
                            feab.pc = a;
                        break;

                    case OP_LESSV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_LESS:
                        if(feab.memory[MEMORY_FLAGS] & FLAG_LESS)
                            feab.pc = a;
                        break;

                    case OP_MOREV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_MORE:
                        if(feab.memory[MEMORY_FLAGS] & FLAG_MORE)
                            feab.pc = a;
                        break;

                    case OP_SETV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_SET:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        feab.memory[a] = b;
                        break;

                    case OP_DEREFV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_DEREF:
                        b = (feab.memory[b] << 8) | (feab.memory[b + 1]);
                        feab.memory[a] = feab.memory[b];
                        break;

                    case OP_REFV:
                        a = (feab.memory[a] << 8) | (feab.memory[a + 1]);
                    case OP_REF:
                        feab.memory[a] = b >> 8, feab.memory[a + 1] = b;
                        break;

                    case OP_ADD:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        if(feab.memory[a] > 0xff - b)
                            {
                                feab.memory[MEMORY_FLAGS] |= FLAG_OVERFLOW;
                                feab.memory[MEMORY_FLAGS] &= ~FLAG_UNDERFLOW;
                            }
                        feab.memory[a] += b;
                        break;

                    case OP_INC:
                        if(feab.memory[a] == 0xff)
                            {
                                feab.memory[MEMORY_FLAGS] |= FLAG_OVERFLOW;
                                feab.memory[MEMORY_FLAGS] &= ~FLAG_UNDERFLOW;
                            }
                        feab.memory[a]++;
                        break;

                    case OP_SUB:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        if(feab.memory[a] < b)
                            {
                                feab.memory[MEMORY_FLAGS] |= FLAG_UNDERFLOW;
                                feab.memory[MEMORY_FLAGS] &= ~FLAG_OVERFLOW;
                            }
                        feab.memory[a] -= b;
                        break;

                    case OP_DEC:
                        if(feab.memory[a] == 0)
                            {
                                feab.memory[MEMORY_FLAGS] |= FLAG_UNDERFLOW;
                                feab.memory[MEMORY_FLAGS] &= ~FLAG_OVERFLOW;
                            }
                        feab.memory[a]--;
                        break;

                    case OP_AND:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        feab.memory[a] &= b;
                        break;

                    case OP_OR:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        feab.memory[a] |= b;
                        break;

                    case OP_NOT:
                        feab.memory[a] = ~feab.memory[a];
                        break;

                    case OP_MOD:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];
                        feab.memory[a] %= b;
                        break;

                    case OP_CMP:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_ADDR_VALUE)
                            a = feab.memory[a];
                        if(mode == MODE_ADDR_ADDR || mode == MODE_VALUE_ADDR)
                            b = feab.memory[b];

                        feab.memory[MEMORY_FLAGS] &= ~(FLAG_EQUAL | FLAG_LESS | FLAG_MORE);

                        if(a == b)
                            feab.memory[MEMORY_FLAGS] |= FLAG_EQUAL;
                        else if(a < b)
                            feab.memory[MEMORY_FLAGS] |= FLAG_LESS;
                        else
                            feab.memory[MEMORY_FLAGS] |= FLAG_MORE;
                        break;

                    case OP_CHAR:
                        if(mode == MODE_ADDR_ADDR || mode == MODE_ADDR_VALUE)
                            a = feab.memory[a];
                        putchar(a);
                        break;
                }
        }

#ifdef DEBUG
    print_memory();
    print_flags();
#endif
}

void
print_memory()
{
    int i;
    printf("[memory]\n");
    for(i = 0; i < MAX_PROGRAM_SIZE; ++i)
        {
            if(i == PROGRAM_START)
                printf("*START* ");
            else if(i == feab.length)
                printf("*END* ");

            printf("%i ", feab.memory[i]);
        }
    printf("\n");
}

void
print_flags()
{
    uint8_t flags = feab.memory[MEMORY_FLAGS];
    printf("[flags] ");

#define FLAG_PRINT(name)\
	if(flags & FLAG_##name)\
		printf(#name " ");

    FLAG_PRINT(QUIT);
    FLAG_PRINT(WAIT);
    FLAG_PRINT(LOAD_SPRITE);
    FLAG_PRINT(EQUAL);
    FLAG_PRINT(LESS);
    FLAG_PRINT(MORE);
    FLAG_PRINT(OVERFLOW);
    FLAG_PRINT(UNDERFLOW);

#undef FLAG_PRINT

    printf("\n");
}

static int
string_length(const char *source)
{
    int i = 0;
    while(source[i++] != '\0');
    return i;
}

static bool
string_ends(const char *a, const char *b)
{
    int length_a = string_length(a) - 1, length_b = string_length(b) - 1;
    int i;

    if(length_a < length_b || length_a == 0 || length_b == 0)
        return false;

    for(i = 0; i < length_b; ++i)
        if(a[length_a - i - 1] != b[length_b - i - 1])
            return false;

    return true;
}

int
cmd(int argc, char *argv[])
{
    if(argc == 1)
        return -1;
    else
        {
            const char *file = argv[1];

            if(string_ends(file, ".rom"))
                load(file);
            else
                {
                    if(string_ends(file, ".asm"))
                        assemble(file);
                    return -1;
                }
        }

    return 0;
}
