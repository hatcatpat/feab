#include "feab.h"

int
string_length(const char *source)
{
    int i = 0;
    while(source[i++] != '\0');
    return i;
}

void
string_copy(const char *source, char *dest, int len)
{
    int i;
    for(i = 0; i < len - 1; ++i)
        dest[i] = source[i];
    dest[i] = '\0';
}

void
string_print_raw(const char *string)
{
    int i = 0;
    for(i = 0; i < string_length(string); ++i)
        printf("[%c %i] ", string[i], string[i]);
}

bool
string_equals(const char *a, const char *b)
{
    int len = string_length(a);

    if(len == string_length(b))
        {
            int i;
            for(i = 0; i < len; ++i)
                if(a[i] != b[i])
                    return false;

            return true;
        }
    else
        return false;
}

int
string_binary_to_number(const char *string)
{
    int i, d = 1, v = 0;
    char c;

    for(i = string_length(string) - 2; i >= 0; i--)
        {
            c = string[i];

            if('0' <= c && c <= '1')
                v += (c - '0') * d;
            else if(c == '.')
                continue;
            else
                return 0;

            d *= 2;
        }

    return v;
}

int
string_to_number(const char *string)
{
    int i, d = 1, v = 0;
    char c;

    for(i = string_length(string) - 2; i >= 0; i--)
        {
            c = string[i];

            if('0' <= c && c <= '9')
                v += (c - '0') * d;
            else
                return 0;

            d *= 10;
        }

    return v;
}

bool
string_is_hex(char *string)
{
    int i;
    for(i = 0; i < string_length(string); ++i)
        if('a' <= string[i] && string[i] <= 'f')
            return true;

    return false;
}

int
string_to_hex(const char *string)
{
    int i;
    int d = 1, v = 0;
    char c;

    for(i = string_length(string) - 2; i >= 0; i--)
        {
            c = string[i];

            if('0' <= c && c <= '9')
                v += (c - '0') * d;
            else if('a' <= c && c <= 'f')
                v += (10 + (c - 'a')) * d;
            else
                return 0;

            d *= 16;
        }

    return v;
}

#define MAX_FILE_LENGTH 1024 * 4

#define MAX_TOKEN_LENGTH (16 + 1)
#define MAX_TOKENS 1024 * 2

#define MAX_LABELS 64
#define MAX_SUBLABELS 16

#define MAX_MACROS 64
#define MAX_MACRO_SIZE 16

enum token_type
{
    TOKEN_NONE,
    TOKEN_SYMBOL,
    TOKEN_NUMBER,
    TOKEN_ADDRESS,
    TOKEN_BINARY,
    TOKEN_LABEL,
    TOKEN_SUBLABEL,
    TOKEN_COMMENT,
    TOKEN_MACRO
};

const char *token_type_names[] =
{
    "NONE",
    "SYMBOL",
    "NUMBER",
    "ADDRESS",
    "BINARY",
    "LABEL",
    "SUBLABEL",
    "COMMENT",
    "MACRO"
};

typedef struct
{
    char string[MAX_TOKEN_LENGTH];
    enum token_type type;
} token_t;

typedef struct
{
    char string[MAX_TOKEN_LENGTH];
    uint16_t address;
} label_t;

typedef struct
{
    label_t label[MAX_SUBLABELS];
    int num_sublabels;
} parent_label_t;

struct
{
    uint8_t data[MAX_PROGRAM_SIZE];
    uint_t length;
} program;

typedef struct
{
    token_t data[MAX_TOKENS];
    uint_t length;
} tokens_t;

tokens_t tokens;

struct
{
    parent_label_t data[MAX_LABELS];
    uint_t length;
} labels;

typedef struct
{
    char string[MAX_TOKEN_LENGTH];
    token_t data[MAX_MACRO_SIZE];
    uint_t length;
} macro_t;

struct
{
    macro_t data[MAX_MACROS];
    uint_t length;
} macros;

void
lexer(char *input, int len);

void
resolve_macros();

void
parser();

void
write(char *file)
{
    FILE *f = fopen(file, "wb");

    if(f == NULL)
        return;

    fwrite(program.data, 1, program.length, f);

    fclose(f);
}

void
assemble(char *file)
{
    char string[MAX_FILE_LENGTH];
    int len;
    FILE *f = fopen(file, "r");

    if(f == NULL)
        return;

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);

    len = MIN(len, MAX_FILE_LENGTH);
    fread(string, 1, len, f);
    string[len - 1] = '\0';

    fclose(f);

    printf("[assemble %s]\n", file);

    lexer(string, len);

    write("output.rom");
}

void
lexer(char *input, int len)
{
    token_t *token = NULL;
    uint_t i = 0, start = 0;
    enum token_type type = TOKEN_NONE;
    bool finish = false;
    char c;

    for(i = 0; i < len; ++i)
        {
            c = input[i];

            switch(c)
                {
                    case '\n':
                    case '\t':
                    case '\0':
                        c = ' ';
                        break;
                }

            switch(type)
                {
                    case TOKEN_NONE:
                        switch(c)
                            {
                                case ' ':
                                    break;

                                case ';':
                                    type = TOKEN_COMMENT;
                                    break;

                                case '#':
                                    type = TOKEN_MACRO;
                                    start = i + 1;
                                    finish = true;
                                    break;

                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                case 'a':
                                case 'b':
                                case 'c':
                                case 'd':
                                case 'e':
                                case 'f':
                                    type = TOKEN_NUMBER;
                                    start = i;
                                    break;

                                case '&':
                                    type = TOKEN_ADDRESS;
                                    start = i + 1;
                                    break;

                                case '%':
                                    type = TOKEN_BINARY;
                                    start = i + 1;
                                    break;

                                default:
                                    type = TOKEN_SYMBOL;
                                    start = i;
                                    break;
                            }
                        break;

                    case TOKEN_COMMENT:
                        if(c == ';' || input[i] == '\n')
                            type = TOKEN_NONE;
                        break;

                    case TOKEN_NUMBER:
                    case TOKEN_ADDRESS:
                        switch(c)
                            {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    break;

                                case 'a':
                                case 'b':
                                case 'c':
                                case 'd':
                                case 'e':
                                case 'f':
                                    if(type == TOKEN_ADDRESS)
                                        {
                                            if(i - start >= 4)
                                                type = TOKEN_SYMBOL;
                                        }
                                    else
                                        {
                                            if(i - start >= 2)
                                                type = TOKEN_SYMBOL;
                                        }
                                    break;

                                case ' ':
                                case ';':
                                    finish = true;
                                    break;

                                default:
                                    type = TOKEN_SYMBOL;
                                    break;
                            }
                        break;

                    case TOKEN_BINARY:
                        if(i - start > 8)
                            finish = true;

                        switch(c)
                            {
                                case '0':
                                case '1':
                                    break;

                                case ' ':
                                case ';':
                                    finish = true;
                                    break;

                                default:
                                    type = TOKEN_NONE;
                                    break;
                            }
                        break;

                    default:
                        switch(c)
                            {
                                case ':':
                                    type = TOKEN_LABEL;
                                case ' ':
                                case ';':
                                    finish = true;
                                    break;
                            }
                        break;
                }

            if(finish)
                {
                    if(i != start)
                        {
                            token = &tokens.data[tokens.length++];
                            string_copy(input + start, token->string, MIN(i - start + 1, MAX_TOKEN_LENGTH));
                            token->type = type;

                            if(type == TOKEN_LABEL && token->string[0] == '@')
                                token->type = TOKEN_SUBLABEL;

                            if(tokens.length >= MAX_TOKENS)
                                break;
                        }

                    type = TOKEN_NONE;
                    finish = false;
                }
        }

    resolve_macros();

    for(i = 0; i < tokens.length; ++i)
        printf("[%i] %s %s\n", i, token_type_names[tokens.data[i].type], tokens.data[i].string);

    parser();
}

macro_t *
macro_find(char *string)
{
    int i;
    for(i = 0; i < macros.length; ++i)
        if(string_equals(macros.data[i].string, string))
            return &macros.data[i];

    return NULL;
}

void
resolve_macros()
{
    int i = 0;
    token_t *token = NULL;
    macro_t *macro = NULL;
    tokens_t old = tokens;
    tokens.length = 0;

    while(i < old.length)
        {
            token = &old.data[i++];

            switch(token->type)
                {
                    case TOKEN_MACRO:
                        if(macro == NULL)
                            {
                                token = &old.data[i++];
                                macro = &macros.data[macros.length++];
                                string_copy(token->string, macro->string, string_length(token->string));
                            }
                        else
                            macro = NULL;
                        break;

                    case TOKEN_SYMBOL:
                    {
                        macro_t *found = macro_find(token->string);

                        if(found != NULL)
                            {
                                int j;
                                for(j = 0; j < found->length; ++j)
                                    {
                                        token = &found->data[j];
                                        tokens.data[tokens.length++] = *token;
                                        if(macro != NULL)
                                            macro->data[macro->length++] = *token;
                                    }
                                break;
                            }
                    }
                    default:
                        if(macro != NULL)
                            macro->data[macro->length++] = *token;
                        else
                            tokens.data[tokens.length++] = *token;
                        break;
                }
        }

    printf("[macros] %i\n", macros.length);
    for(i = 0; i < macros.length; ++i)
        {
            int j;
            printf("\t[%s] ", macros.data[i].string);
            for(j = 0; j < macros.data[i].length; ++j)
                printf("(%s %s) ", token_type_names[macros.data[i].data[j].type], macros.data[i].data[j].string);
            printf("\n");
        }
}

uint8_t
opcode_find(char *string)
{
    int i;
    for(i = 0; i < NUM_OPCODES; ++i)
        if(string_equals(string, opcode[i].string))
            return i;

    return NUM_OPCODES;
}

parent_label_t *
label_find(char *string)
{
    int i;
    for(i = 0; i < labels.length; ++i)
        if(string_equals(labels.data[i].label[0].string, string))
            return &labels.data[i];

    return NULL;
}

label_t *
sublabel_find(parent_label_t *label, char *string)
{
    int i;
    for(i = 0; i < label->num_sublabels; ++i)
        if(string_equals(label->label[1 + i].string, string))
            return &label->label[1 + i];

    return NULL;
}

void
resolve_labels()
{
    int i, bytes = 0;
    token_t *token = NULL;
    parent_label_t *label;

    for(i = 0; i < tokens.length; ++i)
        {
            token = &tokens.data[i];

            switch(token->type)
                {
                    case TOKEN_LABEL:
                        if(label_find(token->string) == NULL)
                            {
                                label = &labels.data[labels.length++];
                                string_copy(token->string, label->label[0].string, string_length(token->string));
                                label->label[0].address = PROGRAM_START + bytes;
                            }
                        else
                            printf("duplicate label! %s\n", token->string);
                        break;

                    case TOKEN_SUBLABEL:
                        if(label != NULL)
                            {
                                if(sublabel_find(label, token->string) == NULL)
                                    {
                                        label_t *sublabel = &label->label[1 + label->num_sublabels++];
                                        string_copy(token->string, sublabel->string, string_length(token->string));
                                        sublabel->address = PROGRAM_START + bytes;
                                    }
                                else
                                    printf("duplicate sublabel! %s\n", token->string);
                            }
                        else
                            printf("sublabel without a scope! %s\n", token->string);
                        break;

                    case TOKEN_NUMBER:
                    case TOKEN_BINARY:
                        bytes++;
                        break;

                    case TOKEN_ADDRESS:
                        bytes += 2;
                        break;

                    case TOKEN_SYMBOL:
                        bytes++;
                        if(opcode_find(token->string) == NUM_OPCODES)
                            bytes++;
                        break;

                    default:
                        break;
                }
        }

    printf("[labels] %i\n", labels.length);
    for(i = 0; i < labels.length; ++i)
        {
            int j;
            for(j = 0; j < label->num_sublabels + 1; ++j)
                {
                    label = &labels.data[i];
                    if(j == 0)
                        printf("\t[%s] %i\n", label->label[j].string, label->label[j].address);
                    else
                        printf("\t\t[%s] %i\n", label->label[j].string, label->label[j].address);
                }
        }
}

void
parser()
{
    token_t *token = NULL;
    parent_label_t *scope = NULL;
    int i;

    resolve_labels();

    for(i = 0; i < tokens.length; ++i)
        {
            token = &tokens.data[i];

            switch(token->type)
                {
                    case TOKEN_LABEL:
                        scope = label_find(token->string);
                        break;

                    case TOKEN_SYMBOL:
                    {
                        uint8_t byte = opcode_find(token->string);

                        if(byte == NUM_OPCODES)
                            {
                                parent_label_t* label = label_find(token->string);
                                label_t *sublabel = NULL;

                                if(label != NULL)
                                    {
                                        uint16_t address = label->label[0].address;
                                        program.data[program.length++] = address >> 8;
                                        program.data[program.length++] = address;
                                    }
                                else if(scope != NULL && (sublabel = sublabel_find(scope, token->string)) != NULL)
                                    {
                                        uint16_t address = sublabel->address;
                                        program.data[program.length++] = address >> 8;
                                        program.data[program.length++] = address;
                                    }
                                else
                                    {
                                        printf("[error] unable to find label %s\n", token->string);
                                        program.data[program.length++] = 0;
                                        program.data[program.length++] = 0;
                                    }
                            }
                        else
                            {
                                uint8_t mode = MODE_VALUE_VALUE;

                                if(opcode[byte].args >= 1 && i < tokens.length - 1)
                                    if(tokens.data[i + 1].type == TOKEN_SYMBOL || tokens.data[i + 1].type == TOKEN_ADDRESS)
                                        mode |= MODE_ADDR_VALUE;

                                if(opcode[byte].args >= 2 && i < tokens.length - 2)
                                    if(tokens.data[i + 2].type == TOKEN_SYMBOL || tokens.data[i + 2].type == TOKEN_ADDRESS)
                                        mode |= MODE_VALUE_ADDR;

                                program.data[program.length++] = byte | (mode << 6);
                            }
                    }
                    break;

                    case TOKEN_NUMBER:
                    case TOKEN_ADDRESS:
                    {
                        uint16_t word = 0;

                        if(string_is_hex(token->string))
                            word = string_to_hex(token->string);
                        else
                            word = string_to_number(token->string);

                        if(token->type == TOKEN_ADDRESS)
                            program.data[program.length++] = word >> 8;

                        program.data[program.length++] = word;
                    }
                    break;

                    case TOKEN_BINARY:
                        program.data[program.length++] = string_binary_to_number(token->string);
                        break;

                    default:
                        break;
                }
        }

    printf("[program] ");
    for(i = 0; i < program.length; ++i)
        {
            printf("%i ", program.data[i]);
            if(i == program.length - 1)
                printf("\n");
        }
}
