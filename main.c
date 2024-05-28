#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint16_t current_line = 0;

uint8_t RA = 0;
uint8_t RB = 0;
uint8_t RC = 0;
uint8_t RD = 0;

uint8_t input_pins = 0;
uint8_t output_pins = 0;

void run_instruction(uint16_t instruction, int *virtual_memory);
uint8_t* find_reg(uint8_t reg);

void set(uint16_t input);
void swp(uint16_t input);
void let(uint16_t input, int *virtual_memory);
void get(uint16_t input, int *virtual_memory);
void add(uint16_t input);
void sub(uint16_t input);
void mul(uint16_t input);
void div_ours(uint16_t input);
void inc(uint16_t input);
void dnc(uint16_t input);
void and(uint16_t input);
void or(uint16_t input);
void not(uint16_t input);
void xor(uint16_t input);
void jmp(uint16_t input);
void cmp(uint16_t input);
void red(uint16_t input);
void wrt(uint16_t input);

int main() 
{
    int *virtual_memory = malloc(512);
    int *file_memory = calloc(2048, 2);

    FILE *fptr;
    fptr = fopen("program.fry", "r");
    char store_file_line[100];
    if(fptr != NULL)
    {
    uint16_t incrementer = 0;
    while(fgets(store_file_line, 100, fptr)) {
        uint16_t instruction = atoi(store_file_line);
        file_memory[incrementer] = instruction;
        incrementer++;
    }
    } else 
    {
        printf("cannot open file\n");
    }
    fclose(fptr);

    while(1)
    {
        printf("Instruction: %d\n", file_memory[current_line]);
        run_instruction(file_memory[current_line], virtual_memory);
        printf("RA: %d, RB: %d, RC: %d, RD %d\n", RA, RB, RC, RD);
        printf("Input Pins: %d\n", input_pins);
        printf("Output Pins: %d\n\n", output_pins);
        current_line++;
    }

    free(virtual_memory);
    free(file_memory);
    return 0;
}

void wrt(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;

    uint8_t *reg = find_reg(input1);

    printf("INPUT_PINS> ");
    scanf("%d", reg);
    input_pins = *reg;
}

void red(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;

    uint8_t *reg = find_reg(input1);

    output_pins = *reg;
}

void cmp(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011100000000) >> 8;
    uint8_t input2 = (input & 0b0000000011000000) >> 6;
    uint8_t input3 = (input & 0b0000000000110000) >> 4;

    uint8_t *reg1 = find_reg(input2);
    uint8_t *reg2 = find_reg(input3);

    switch(input1)
    {
        case 0:
            if (!(*reg1 > *reg2))
            {
                current_line++;
            }
            break;
        case 1:
            if (!(*reg1 < *reg2))
            {
                current_line++;
            }
            break;
        case 2:
            if (!(*reg1 == *reg2))
            {
                current_line++;
            }
            break;
        case 3:
            if (!(*reg1 >= *reg2))
            {
                current_line++;
            }
            break;
        case 4:
            if (!(*reg1 <= *reg2))
            {
                current_line++;
            }
            break;
        case 5:
            if (!(*reg1 != *reg2))
            {
                current_line++;
            }
            break;
        default:
            printf("could not find operator\n");
            break;
    }
}

void jmp(uint16_t input)
{
    uint8_t input1 = input & 0b0000011111111111;
    current_line = input1;
}

void xor(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 ^ *reg2;
}

void not(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);

    *reg2 = ~(*reg1);
}

void or(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 | *reg2;
}

void and(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 & *reg2;
}

void dnc(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;

    uint8_t *reg = find_reg(input1);
    *reg -= 1;
}

void inc(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;

    uint8_t *reg = find_reg(input1);
    *reg += 1;
}

void div_ours(uint16_t input) 
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 / *reg2;
}

void mul(uint16_t input) 
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = (*reg1) * (*reg2);
}

void sub(uint16_t input) 
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 - *reg2;
}

void add(uint16_t input) 
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;
    uint8_t input3 = (input & 0b0000000001100000) >> 5;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t *reg3 = find_reg(input3);

    *reg3 = *reg1 + *reg2;
}

void get(uint16_t input, int *virtual_memory)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint16_t input2 = input & 0b0000000111111111;

    uint8_t *reg = find_reg(input1);
    *reg = virtual_memory[input2];
}

void let(uint16_t input, int *virtual_memory)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint16_t input2 = input & 0b0000000111111111;

    uint8_t *reg = find_reg(input1);
    virtual_memory[input2] = *reg;
}

void swp(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000110000000) >> 7;

    uint8_t *reg1 = find_reg(input1);
    uint8_t *reg2 = find_reg(input2);
    uint8_t temp = *reg1;
    *reg1 = *reg2;
    *reg2 = temp;
}

void set(uint16_t input)
{
    uint8_t input1 = (input & 0b0000011000000000) >> 9;
    uint8_t input2 = (input & 0b0000000111111110) >> 1;

    uint8_t *reg = find_reg(input1);
    *reg = input2;
}

uint8_t* find_reg(uint8_t reg)
{
    switch(reg)
    {
        case 0:
            return &RA;
            break;
        case 1:
            return &RB;
            break;
        case 2:
            return &RC;
            break;
        case 3:
            return &RD;
            break;
        default:
            printf("cannot find registor\n");
            return NULL;
            break;
    }
    return NULL;
}

void run_instruction(uint16_t instruction, int *virtual_memory)
{
    uint8_t command = (instruction & 0b1111100000000000) >> 11;
    uint16_t input = instruction & 0b0000011111111111;

    switch(command)
    {
        case 0:
            break;
        case 1:
            set(input);
            break;
        case 2:
            swp(input);
            break;
        case 3:
            let(input, virtual_memory);
            break;
        case 4:
            get(input, virtual_memory);
            break;
        case 5:
            add(input);
            break;
        case 6:
            sub(input);
            break;
        case 7:
            mul(input);
            break;
        case 8:
            div_ours(input);
            break;
        case 9:
            inc(input);
            break;
        case 10:
            dnc(input);
            break;
        case 11:
            and(input);
            break;
        case 12:
            or(input);
            break;
        case 13:
            not(input);
            break;
        case 14:
            xor(input);
            break;
        case 15:
            jmp(input);
            break;
        case 16:
            cmp(input);
            break;
        case 17:
            red(input);
            break;
        case 18:
            wrt(input);
            break;
        case 19:
            free(virtual_memory);
            exit(0);
            break;
        default:
            printf("command not recognized\n");
            break;
    }
}