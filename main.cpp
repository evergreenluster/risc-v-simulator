#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdint>
#include "cpu_single_hart.h"
#include "rv32i_decode.h"
#include "rv32i_hart.h"
#include "memory.h"
#include "hex.h"

// UPDATE USAGE() (SEE DOC)

/**
 * @brief Disassembles contents of simulated memory into RISC-V assembly
 * @param mem Reference to the memory object containing instructions
 * @details Loops through memory in 4-byte increments, decoding each
 *          32-bit word as a RISC-V instruction. For each instruction:
 *          - Prints the memory address in hex
 *          - Prints the instruction word in hex
 *          - Prints the decoded assembly instruction
 */
static void disassemble(const memory &mem) 
{
    for (size_t i = 0; i < mem.get_size(); i += 4)
    {
        uint32_t insn = mem.get32(i);
        // print the starting address of the current 4-byte line
        std::cout << hex::to_hex32(i) << ": " << hex::to_hex32(insn) << "  "
                  << rv32i_decode::decode(i, insn) << "\n";           
    }
}

/**
 * @brief Displays usage information and exits program
 * @details Prints the correct command line format for the program
 *          and exits with status code 1 to indicate error
 */
static void usage()
{
    std::cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit]" 
              << "[-m hex-mem-size] infile" << std::endl;
    std::cerr << "    -d show disassembly before program execution"
              << "    -i show instruction printing during execution"
              << "    -r show register printing during execution"
              << "    -z show a dump of the regs & memory after simulation"
              << "    -l maximum number of instructions to exec"
              << "    -m specify memory size (default = 0x100)" 
              << std::endl;
    exit(1);
}

/**
 * @brief Main program for memory simulator
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return 0 on success, 1 on error
 */ 
int main(int argc, char **argv) 
{
    uint32_t memory_limit = 0x100;  // default memory size = 256 bytes
    uint64_t exec_limit = 0;        // no limit when set to zero

    int opt;
    bool d = false, i = false, r = false, z = false;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "dirzl:m:")) != -1) 
    {
        switch (opt) 
        {   
            case 'd':  // show disassembled memory before simulation begins option
            {
                d = true;
                break;
            }
            case 'i':  // show instruction printing during execution option
            {
                i = true;
                break;
            }
            case 'r':  // show a dump of the hart status during execution option
            {
                r = true;
                break;
            }
            case 'z':  // show and dump of the hart status and memory after simulation option
            {
                z = true;
                break;
            }
            case 'l': // set execution limit option
            {
                std::istringstream iss(optarg);
                iss >> std::hex >> exec_limit;  // read limit as hex
                break;
            }
            case 'm':  // set memory size option 
            {
                std::istringstream iss(optarg);
                iss >> std::hex >> memory_limit;  // read size as hex
                break;
            }
            default: /* '?' */
                usage();
        }
    }
    
    if (optind >= argc)
        usage();  // missing filename
        
    memory mem(memory_limit);  // create memory object
    
    // load file contents into memory
    if (!mem.load_file(argv[optind])) 
    {
        usage();
    }

    if (d)
    {
        disassemble(mem);  // disassemble memory contents into RISC-V assembly
        mem.dump();        // display memory after loading file
    }

    // create and configure cpu
    cpu_single_hart cpu(mem);  
    cpu.reset();

    cpu.set_show_instructions(i);  // set flag to show instructions
    cpu.set_show_registers(r);     // set flag to show registers
    
    cpu.run(exec_limit);  // run program

    if (z)
    {
        cpu.dump();  // dump hart status
        mem.dump();  // dump memory
    }

    return 0;
}