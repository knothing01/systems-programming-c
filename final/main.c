#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define RAM_SIZE        65536
#define PAGE_SIZE       256
#define NUM_PAGES       (RAM_SIZE / PAGE_SIZE)
#define TLB_SIZE        16
#define HDD_SIZE        1048576
#define STACK_START     (RAM_SIZE - 1)

typedef uint8_t  byte;
typedef uint16_t word;

// Forward declarations
void print_memory_stats(void);
void print_tlb_stats(void);

// CPU register/file state
static struct {
    word PC;
    word SP;
    byte R[4];    // AYB, BEN, GIM, DAH
    byte TMP[4];  // TMP0-TMP3
    byte FLAGS;   // bit0: zero, bit1: sign
} regs;

// Statistics
static unsigned long stats_instructions = 0;
static unsigned long stats_mem_access   = 0;
static unsigned long stats_tlb_hits      = 0;
static unsigned long stats_tlb_misses   = 0;
static unsigned long stats_page_faults  = 0;

// Memory and translation structures
typedef struct { int valid, frame; } PTE;
typedef struct { int valid, vp, frame; } TLBEntry;
static byte     ram[RAM_SIZE];
static byte     hdd[HDD_SIZE];
static PTE      page_table[NUM_PAGES];
static TLBEntry tlb[TLB_SIZE];
static int      next_frame = 0;
static int      next_tlb   = 0;

// Modes
static int debug   = 0;
static int verbose = 0;

// OPCODE definitions
enum OPCODES {
    OP_NOP  = 0x03,
    OP_MOV  = 0x10,
    OP_MOVI = 0x11,
    OP_ADD  = 0x20,
    OP_ADDI = 0x21,
    OP_SUB  = 0x30,
    OP_SUBI = 0x31,
    OP_CMP  = 0x40,
    OP_CMPI = 0x41,
    OP_JMP  = 0x50,
    OP_JZ   = 0x51,
    OP_JNZ  = 0x52,
    OP_AND  = 0x60,
    OP_OR   = 0x61,
    OP_NOT  = 0x62,
    OP_LDR  = 0x70,
    OP_STR  = 0x71,
    OP_PUSH = 0x80,
    OP_POP  = 0x81,
    OP_CALL = 0x90,
    OP_RET  = 0x91,
    OP_HALT = 0xFF
};

// Helper: dump registers
void dump_registers(void) {
    printf("\n=== REGISTER DUMP ===\n");
    printf("AYB (R0): %u\n", regs.R[0]);
    printf("BEN (R1): %u\n", regs.R[1]);
    printf("GIM (R2): %u\n", regs.R[2]);
    printf("DAH (R3): %u\n", regs.R[3]);
    printf("TMP0   : %u\n", regs.TMP[0]);
    printf("TMP1   : %u\n", regs.TMP[1]);
    printf("TMP2   : %u\n", regs.TMP[2]);
    printf("TMP3   : %u\n", regs.TMP[3]);
    printf("PC      : 0x%04X\n", regs.PC);
    printf("SP      : 0x%04X\n", regs.SP);
    printf("FLAGS Z,S: %d,%d\n\n", (regs.FLAGS & 1), ((regs.FLAGS >> 1) & 1));
}

// Address translation and memory access
word translate_address(word vaddr, int write) {
    stats_mem_access++;
    word vpn = vaddr / PAGE_SIZE;
    word offset = vaddr % PAGE_SIZE;
    int frame = -1;
    // TLB lookup
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].vp == vpn) {
            stats_tlb_hits++;
            frame = tlb[i].frame;
            break;
        }
    }
    if (frame < 0) {
        stats_tlb_misses++;
        // Page fault handler
        if (!page_table[vpn].valid) {
            stats_page_faults++;
            page_table[vpn].valid = 1;
            page_table[vpn].frame = next_frame++;
            memcpy(ram + page_table[vpn].frame * PAGE_SIZE,
                   hdd + vpn * PAGE_SIZE,
                   PAGE_SIZE);
        }
        frame = page_table[vpn].frame;
        tlb[next_tlb % TLB_SIZE] = (TLBEntry){1, vpn, frame};
        next_tlb++;
    }
    return frame * PAGE_SIZE + offset;
}

byte read_byte(word addr) {
    return ram[translate_address(addr, 0)];
}

void write_byte(word addr, byte val) {
    ram[translate_address(addr, 1)] = val;
}

byte fetch_byte(void) {
    return read_byte(regs.PC++);
}

word fetch_word(void) {
    word hi = read_byte(regs.PC);
    word lo = read_byte(regs.PC + 1);
    regs.PC += 2;
    return (hi << 8) | lo;
}

void push_word(word v) {
    write_byte(regs.SP--, (v >> 8) & 0xFF);
    write_byte(regs.SP--, v & 0xFF);
}

word pop_word(void) {
    byte lo = read_byte(++regs.SP);
    byte hi = read_byte(++regs.SP);
    return (hi << 8) | lo;
}

byte* get_register(int r) {
    if (r < 4) return &regs.R[r];
    if (r < 8) return &regs.TMP[r - 4];
    return NULL;
}

// Stubs for external stats printing
void print_memory_stats(void) {
    printf("Memory accesses: %lu\n", stats_mem_access);
    printf("Page faults   : %lu\n", stats_page_faults);
}

void print_tlb_stats(void) {
    printf("TLB hits      : %lu\n", stats_tlb_hits);
    printf("TLB misses    : %lu\n\n", stats_tlb_misses);
}

// Load and initialize
void load_program(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) { perror("Failed to open program"); exit(1); }
    size_t n = fread(hdd, 1, HDD_SIZE, f);
    fclose(f);
    if (verbose) {
        printf("Loaded %zu bytes\nFirst 16 bytes: ", n);
        for (int i = 0; i < 16; i++) printf("%02X ", hdd[i]);
        printf("\n");
    }
    if (n == 0) { fprintf(stderr, "Empty program file\n"); exit(1); }
}

void initialize_system(void) {
    memset(ram, 0, RAM_SIZE);
    memset(page_table, 0, sizeof(page_table));
    memset(tlb, 0, sizeof(tlb));
    regs.PC = 0;
    regs.SP = STACK_START;
    memset(regs.R, 0, sizeof regs.R);
    memset(regs.TMP, 0, sizeof regs.TMP);
    regs.FLAGS = 0;
    // Map and load first page
    page_table[0].valid = 1;
    page_table[0].frame = 0;
    memcpy(ram, hdd, PAGE_SIZE);
    next_frame = 1;
    if (verbose) printf("System initialized\n");
}

// Execute one instruction
void execute_instruction(byte opc) {
    stats_instructions++;
    byte spec;
    int a, b;
    if (verbose) printf("Exec 0x%02X @ 0x%04X\n", opc, regs.PC-1);
    switch (opc) {
        case OP_NOP:
            break;
        case OP_MOV:
            spec = fetch_byte();
            *get_register(spec >> 4) = *get_register(spec & 0xF);
            break;
        case OP_MOVI:
            spec = fetch_byte();
            *get_register(spec >> 4) = fetch_byte();
            break;
        case OP_ADD:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = *get_register(spec & 0xF);
            *get_register(spec >> 4) = a + b;
            regs.FLAGS = ((a+b)==0) | (((a+b)&0x80)?2:0);
            break;
        case OP_ADDI:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = fetch_byte();
            *get_register(spec >> 4) = a + b;
            regs.FLAGS = ((a+b)==0) | (((a+b)&0x80)?2:0);
            break;
        case OP_SUB:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = *get_register(spec & 0xF);
            *get_register(spec >> 4) = a - b;
            regs.FLAGS = ((a-b)==0) | (((a-b)&0x80)?2:0);
            break;
        case OP_SUBI:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = fetch_byte();
            *get_register(spec >> 4) = a - b;
            regs.FLAGS = ((a-b)==0) | (((a-b)&0x80)?2:0);
            break;
        case OP_CMP:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = *get_register(spec & 0xF);
            regs.FLAGS = ((a-b)==0) | (((a-b)&0x80)?2:0);
            break;
        case OP_CMPI:
            spec = fetch_byte();
            a = *get_register(spec >> 4);
            b = fetch_byte();
            regs.FLAGS = ((a-b)==0) | (((a-b)&0x80)?2:0);
            break;
        case OP_JMP:
            regs.PC = fetch_word();
            break;
        case OP_JZ:
            if (regs.FLAGS & 1) regs.PC = fetch_word(); else regs.PC += 2;
            break;
        case OP_JNZ:
            if (!(regs.FLAGS & 1)) regs.PC = fetch_word(); else regs.PC += 2;
            break;
        case OP_AND:
            spec = fetch_byte();
            *get_register(spec >> 4) &= *get_register(spec & 0xF);
            break;
        case OP_OR:
            spec = fetch_byte();
            *get_register(spec >> 4) |= *get_register(spec & 0xF);
            break;
        case OP_NOT:
            spec = fetch_byte();
            *get_register(spec & 0xF) = ~*get_register(spec & 0xF);
            break;
        case OP_LDR:
            spec = fetch_byte();
            *get_register(spec >> 4) = read_byte(fetch_word());
            break;
        case OP_STR:
            spec = fetch_byte();
            write_byte(fetch_word(), *get_register(spec >> 4));
            break;
        case OP_PUSH:
            push_word(*get_register(fetch_byte() & 0xF));
            break;
        case OP_POP:
            *get_register(fetch_byte() & 0xF) = pop_word();
            break;
        case OP_CALL:
            push_word(regs.PC + 2);
            regs.PC = fetch_word();
            break;
        case OP_RET:
            regs.PC = pop_word();
            break;
        case OP_HALT:
            // HALT handling
            printf("Execution halted at PC=0x%04X\n", regs.PC);
            printf("Instructions: %lu\n", stats_instructions);
            if (verbose) {
                dump_registers();
                print_memory_stats();
                print_tlb_stats();
            }
            exit(0);
            break;
        default:
            fprintf(stderr, "Invalid opcode 0x%02X at PC=0x%04X\n", opc, regs.PC-1);
            exit(1);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-d] [-v] program.bin\n", argv[0]);
        return 1;
    }
    int idx = 1;
    while (idx < argc - 1) {
        if (strcmp(argv[idx], "-d") == 0) debug = 1;
        else if (strcmp(argv[idx], "-v") == 0) verbose = 1;
        idx++;
    }
    load_program(argv[idx]);
    initialize_system();
    printf("Starting execution at PC=0x%04X\n", regs.PC);
    while (1) {
        byte opc = read_byte(regs.PC++);
        execute_instruction(opc);
    }
    return 0;
}
