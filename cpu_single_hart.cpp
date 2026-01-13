#include "cpu_single_hart.h"

void cpu_single_hart::run(uint64_t exec_limit)
{
    // Initialize the stack pointer (x2) to point to the top of memory
    // This gives programs a reasonable starting point for stack operations
    regs.set(2, mem.get_size());
    
    // Determine what header strings to use based on user's debug flags
    // If show_instructions is enabled, exec() will display each instruction as it executes
    // If show_registers is enabled, dump() will display register state after each instruction
    std::string instruction_hdr = get_show_instructions() ? " " : "";
    std::string register_hdr = get_show_registers() ? " " : "";
    
    // Determine execution mode based on exec_limit parameter
    if (exec_limit == 0) {
        // Unlimited execution mode: run until the program halts naturally
        // This continues until the program executes a halt instruction,
        // encounters an illegal instruction, or hits some other stopping condition
        while (!is_halted()) {
            exec(regs.get(2));  // Execute one instruction, showing it if requested
            if (get_show_registers()) {
                dump(register_hdr);  // Show register state if requested
            }
        }
    } else {
        // Limited execution mode: run for a specific number of instructions
        // This loop has two exit conditions:
        // 1. We've executed the requested number of instructions (exec_limit)
        // 2. The program has halted before reaching the limit
        for (uint64_t i = 0; i < exec_limit && !is_halted(); ++i) {
            exec(instruction_hdr);  // Execute one instruction, showing it if requested
            if (get_show_registers()) {
                dump(register_hdr);  // Show register state if requested
            }
        }
    }
    
    // Report why execution stopped (only relevant if the program halted)
    // This helps with debugging - did the program end normally or due to an error?
    if (is_halted()) {
        std::cout << "Execution terminated. Reason: " << get_halt_reason() << std::endl;
    }
    
    // Always report the total number of instructions executed
    // This is useful for performance analysis and verification
    std::cout << get_insn_counter() << " instructions executed" << std::endl;
}