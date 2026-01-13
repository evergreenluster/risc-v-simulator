#ifndef _RV32I_HART_
#define _RV32I_HART_

#include <iostream>
#include <cstdint>
#include <iomanip>
#include <cassert>
#include "memory.h"
#include "rv32i_decode.h"
#include "registerfile.h"

class rv32i_hart : public rv32i_decode
{
    public:
        // see documentation
        rv32i_hart(memory &m) : mem(m) {}

        // Mutator for show_instructions. When true, show each instruction 
        // that is executed with a comment displaying the register values used.
        void set_show_instructions(bool b) 
        { 
            show_instructions = b; 
        }

        bool get_show_instructions() const 
        { 
            return show_instructions; 
        }

        //  Mutator for show_registers. When true, dump the registers before instruction is executed.
        void set_show_registers(bool b)
        {
            show_registers = b;
        }

        // Add this to the public section of rv32i_hart class
        bool get_show_registers() const 
        { 
            return show_registers; 
        }

        // Accessor for halt. Return true if the hart has been halted for any reason.
        bool is_halted() const
        {
            return halt;
        }

        // see documentation
        const std::string &get_halt_reason() const
        {
            return halt_reason;
        }

        // Accessor for insn_counter. Return the number of instructions that have been 
        // executed by the simulator since the last reset().
        uint64_t get_insn_counter() const
        {
            return insn_counter;
        }

        // Mutator for mhartid. This is used to set the ID value to be returned by the 
        // csrrs instruction for CSR register number 0xf14. This will always be zero on
        // processors that only have a single-hart (see doc).
        void set_mhartid(int i)
        {
            mhartid = i;
        }

        // see documentation
        void tick(const std::string &hdr="");

        // see documentation
        void dump(const std::string &hdr="") const;

        // Reset the rv32i object and the registerfile (see doc).
        void reset();        

    private:
        static constexpr int instruction_width = 35;

        // similar to decode(...), see doc.
        void exec(uint32_t insn, std::ostream* pos);

        // respective function to each render function
        // will need one function for each instruction... like render_xxx().
        void exec_illegal_insn(uint32_t insn, std::ostream* pos); // check doc
        void exec_lui(uint32_t insn, std::ostream* pos);
        void exec_auipc(uint32_t insn, std::ostream* pos);
        void exec_jal(uint32_t insn, std::ostream* pos);
        void exec_jalr(uint32_t insn, std::ostream* pos);
        void exec_beq(uint32_t insn, std::ostream* pos);
        void exec_bne(uint32_t insn, std::ostream* pos);
        void exec_blt(uint32_t insn, std::ostream* pos);
        void exec_bge(uint32_t insn, std::ostream* pos);
        void exec_bltu(uint32_t insn, std::ostream* pos);
        void exec_bgeu(uint32_t insn, std::ostream* pos);
        void exec_lb(uint32_t insn, std::ostream* pos);
        void exec_lh(uint32_t insn, std::ostream* pos);
        void exec_lw(uint32_t insn, std::ostream* pos);
        void exec_lbu(uint32_t insn, std::ostream* pos);
        void exec_lhu(uint32_t insn, std::ostream* pos);
        void exec_sb(uint32_t insn, std::ostream* pos);
        void exec_sh(uint32_t insn, std::ostream* pos);
        void exec_sw(uint32_t insn, std::ostream* pos);
        void exec_addi(uint32_t insn, std::ostream* pos);
        void exec_slti(uint32_t insn, std::ostream* pos);
        void exec_sltiu(uint32_t insn, std::ostream* pos);
        void exec_xori(uint32_t insn, std::ostream* pos);
        void exec_ori(uint32_t insn, std::ostream* pos);
        void exec_andi(uint32_t insn, std::ostream* pos);
        void exec_slli(uint32_t insn, std::ostream* pos);
        void exec_srli(uint32_t insn, std::ostream* pos);
        void exec_srai(uint32_t insn, std::ostream* pos);
        void exec_add(uint32_t insn, std::ostream* pos);
        void exec_sub(uint32_t insn, std::ostream* pos);
        void exec_sll(uint32_t insn, std::ostream* pos);
        void exec_slt(uint32_t insn, std::ostream* pos);
        void exec_sltu(uint32_t insn, std::ostream* pos);
        void exec_xor(uint32_t insn, std::ostream* pos);
        void exec_srl(uint32_t insn, std::ostream* pos);
        void exec_sra(uint32_t insn, std::ostream* pos);
        void exec_or(uint32_t insn, std::ostream* pos);
        void exec_and(uint32_t insn, std::ostream* pos);
        void exec_ecall(uint32_t insn, std::ostream* pos);
        void exec_ebreak(uint32_t insn, std::ostream* pos);
        void exec_csrrw(uint32_t insn, std::ostream* pos);
        void exec_csrrs(uint32_t insn, std::ostream* pos);
        void exec_csrrc(uint32_t insn, std::ostream* pos);
        void exec_csrrwi(uint32_t insn, std::ostream* pos);
        void exec_csrrsi(uint32_t insn, std::ostream* pos);
        void exec_csrrci(uint32_t insn, std::ostream* pos);

        // A flag with a default value of false. When true, print each instruction when simulating its execution.
        bool show_instructions = false;

        //  A flag with a default value of false. When true, print a dump of the hart state (by calling dump())
        //  before executing each instruction
        bool show_registers = false;

        // A flag to stop the hart from executing instructions. Set it any time that the execution should halt and
        // use it in tick() to prevent further instructions from executing until/unless reset() is invoked.
        bool halt = false;

        // If halt is set to true, also set this to contain a string describing the reason for the halt. Initialize to
        // "none" if reset() is called.
        std::string halt_reason = "none";

        // This contains the CSR register value to return by a csrrs instruction that reads register 0xf14. Set
        // the default value for this to zero. (In this assignment, this default value will never change.)
        uint32_t mhartid = 0;

        // This will count the number of instructions that have been executed. Initialize to zero and if/when reset() is called.
        // Use this to count the number of instructions executed.
        uint64_t insn_counter = 0;

        // Use this to contain the address of the instruction being decoded/disassembled. When decoding in
        // structions that refer to the pc register to calculate a target address (e.g. auipc, jal, and branch
        // instructions) use this value to determine the instructions memory address.
        // Initialize to zero and if/when reset() is called
        uint32_t pc = 0;

    protected:
        registerfile regs;
        memory &mem;
};

#endif