# CPU Simulator

## Overview

This is a simple, pedagogical CPU simulator written in C. It models:

- **Registers**  
  - Four general-purpose 8-bit registers: AYB (R0), BEN (R1), GIM (R2), DAH (R3)  
  - Four temporary registers: TMP0–TMP3  
  - Program Counter (PC) and Stack Pointer (SP)  
  - Two flag bits (Zero and Sign) in an 8-bit FLAGS register

- **Memory**  
  - 64 KB of byte-addressable RAM  
  - Virtual memory divided into 256 B pages, backed by a 1 MB HDD image  
  - A 16-entry fully associative TLB with round-robin replacement  
  - On a page fault, the page is loaded from the HDD image into RAM

- **Instruction Set**  
  - Data movement: `MOV`, `MOVI`  
  - Arithmetic/logic: `ADD`, `ADDI`, `SUB`, `SUBI`, `AND`, `OR`, `NOT`  
  - Compare/test: `CMP`, `CMPI`  
  - Control flow: `JMP`, `JZ`, `JNZ`, `CALL`, `RET`, `HALT`  
  - Memory access: `LDR`, `STR`  
  - Stack operations: `PUSH`, `POP`  
  - No-op: `NOP`

- **Debug & Statistics**  
  - **Debug mode** prints each instruction as it executes.  
  - **Verbose mode** (used together with debug) also dumps the full register state, memory-access statistics, TLB hits/misses, and page-fault count when execution halts.  

---

## Files

- **`cpu_simulator.c`**  
  The full C source code of the simulator.

- **`program.bin`**  
  A binary file containing your machine-code instructions.  
  > **Note:** We’ll provide you with a sample `program.bin`. It encodes:  
  > 1. Load immediate 5 into R0 (AYB)  
  > 2. Load immediate 10 into R1 (BEN)  
  > 3. Add R1 into R0  
  > 4. HALT  
  >  
  > When you run it, you should see R0 = 15 at the end.

- **`README.md`**  
  This file.

---

## Prerequisites

- A Unix-like environment (Linux, macOS) or Windows with a POSIX-compatible shell.  
- A C compiler supporting C99 (e.g. `gcc`, `clang`).

---

## Building

```bash
gcc -std=c99 -O2 -o cpu_simulator cpu_simulator.c
