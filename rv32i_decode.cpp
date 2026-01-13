#include "rv32i_decode.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>

/**
 * @brief Decodes a RISC-V instruction into assembly language
 * @param addr The memory address where the instruction is stored
 * @param insn The 32-bit instruction to decode
 * @return String containing the assembly instruction
 * @details Uses a switch statement hierarchy to identify instruction type
 *          and calls appropriate render functions for each instruction format
 */
std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
    switch (get_opcode(insn))
    {
        default:                            return render_illegal_insn(insn);
        case opcode_lui:                    return render_lui(insn);
        case opcode_auipc:                  return render_auipc(insn);
        case opcode_jal:                    return render_jal(addr, insn);
        case opcode_jalr:                   return render_jalr(insn);
        case opcode_btype:
            switch(get_funct3(insn))
            {
                default:                    return render_illegal_insn(insn);
                case funct3_beq:            return render_btype(addr, insn, "beq");
                case funct3_bne:            return render_btype(addr, insn, "bne");
                case funct3_blt:            return render_btype(addr, insn, "blt");
                case funct3_bge:            return render_btype(addr, insn, "bge");
                case funct3_bltu:           return render_btype(addr, insn, "bltu");
                case funct3_bgeu:           return render_btype(addr, insn, "bgeu");
            }
            assert(0 && "unrecognized funct3"); // impossible
        case opcode_load_imm:           
            switch(get_funct3(insn))
            {
                default:                    return render_illegal_insn(insn);
                case funct3_lb:             return render_itype_load(insn, "lb");
                case funct3_lh:             return render_itype_load(insn,"lh");
                case funct3_lw:             return render_itype_load(insn, "lw");
                case funct3_lbu:            return render_itype_load(insn, "lbu");
                case funct3_lhu:            return render_itype_load(insn, "lhu");
            }
            assert(0 && "unrecognized funct3"); // impossible
        case opcode_stype:
            switch(get_funct3(insn))
            {   
                default:                    return render_illegal_insn(insn);
                case funct3_sb:             return render_stype(insn, "sb");
                case funct3_sh:             return render_stype(insn, "sh");
                case funct3_sw:             return render_stype(insn, "sw");
            }
            assert(0 && "unrecognized funct3"); // impossible
        case opcode_alu_imm:
            switch(get_funct3(insn))
            {
                default:                    return render_illegal_insn(insn);
                case funct3_add:            return render_itype_alu(insn, "addi", get_imm_i(insn));
                case funct3_slt:            return render_itype_alu(insn, "slti", get_imm_i(insn));
                case funct3_sltu:           return render_itype_alu(insn, "sltiu", get_imm_i(insn));
                case funct3_xor:            return render_itype_alu(insn, "xori", get_imm_i(insn));
                case funct3_or:             return render_itype_alu(insn, "ori", get_imm_i(insn));
                case funct3_and:            return render_itype_alu(insn, "andi", get_imm_i(insn));
                case funct3_sll:            return render_itype_alu(insn, "slli", get_imm_i(insn)%XLEN);
                case funct3_srx:     
                    switch(get_funct7(insn))
                    {
                        default:            return render_illegal_insn(insn);
                        case funct7_srl:    return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN); 
                        case funct7_sra:    return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
                    }
                    assert(0 && "unrecognized funct7"); // impossible
            }
            assert(0 && "unrecognized funct3"); // impossible
        case opcode_rtype:
            switch(get_funct3(insn))
            {
                default:                    return render_illegal_insn(insn);
                case funct3_add:
                    switch(get_funct7(insn))
                    {   
                        default:            return render_illegal_insn(insn);
                        case funct7_add:    return render_rtype(insn, "add");
                        case funct7_sub:    return render_rtype(insn, "sub");
                    }
                    assert(0 && "unrecognized funct7"); // impossible
                case funct3_sll:            return render_rtype(insn, "sll");
                case funct3_slt:            return render_rtype(insn,"slt");
                case funct3_sltu:           return render_rtype(insn,"sltu");
                case funct3_xor:            return render_rtype(insn,"xor");
                case funct3_srx:
                    switch(get_funct7(insn))
                    {
                        default:            return render_illegal_insn(insn);
                        case funct7_srl:    return render_rtype(insn,"srl");
                        case funct7_sra:    return render_rtype(insn,"sra");
                    }
                    assert(0 && "unrecognized funct7"); // impossible
                case funct3_or:             return render_rtype(insn,"or");
                case funct3_and:            return render_rtype(insn,"and");
            }
            assert(0 && "unrecognized funct3"); // impossible
        case opcode_system:
            switch(insn)
            {
                case insn_ecall:            return render_ecall(insn);
                case insn_ebreak:           return render_ebreak(insn);
                default:
                    switch(get_funct3(insn))
                    {   
                        default:            return render_illegal_insn(insn);
                        case funct3_csrrw:  return render_csrrx(insn, "csrrw");
                        case funct3_csrrs:  return render_csrrx(insn, "csrrs");
                        case funct3_csrrc:  return render_csrrx(insn, "csrrc");
                        case funct3_csrrwi: return render_csrrxi(insn, "csrrwi");
                        case funct3_csrrsi: return render_csrrxi(insn, "csrrsi");
                        case funct3_csrrci: return render_csrrxi(insn, "csrrci");
                    }
                    assert(0 && "unrecognized funct3"); // impossible
            }
            assert(0 && "unrecognized insn"); // impossible
    }
    assert(0 && "unrecognized opcode"); // impossible
}

/**
 * @brief Gets the opcode field from an instruction
 * @param insn The 32-bit instruction
 * @return The opcode value
 * @details Extracts bits 0-6 of the instruction
 */
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return (insn & 0x0000007f);
}

/**
 * @brief Gets the rd (destination register) field from an instruction
 * @param insn The 32-bit instruction
 * @return The rd value (0-31)
 * @details Extracts bits 7-11 of the instruction
 */
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    uint32_t rd = (insn & 0x00000f80) >> (7-0);

    return rd;
}

/**
 * @brief Gets the rs1 (first source register) field from an instruction
 * @param insn The 32-bit instruction
 * @return The rs1 value (0-31)
 * @details Extracts bits 15-19 of the instruction
 */
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    uint32_t rs1 = (insn & 0x000f8000) >> (15 - 0);

    return rs1;
}

/**
 * @brief Gets the rs2 (second source register) field from an instruction
 * @param insn The 32-bit instruction
 * @return The rs2 value (0-31)
 * @details Extracts bits 20-24 of the instruction
 */
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    uint32_t rs2 = (insn & 0x01f00000) >> (20 - 0);

    return rs2;
}

/**
 * @brief Gets the funct3 field from an instruction
 * @param insn The 32-bit instruction
 * @return The funct3 value (0-7)
 * @details Extracts bits 12-14 of the instruction, used to further
 *          specify the instruction type
 */
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    uint32_t funct3 = (insn & 0x00007000) >> (12 - 0);

    return funct3;
}

/**
 * @brief Gets the funct7 field from an instruction
 * @param insn The 32-bit instruction
 * @return The funct7 value (0x00-0x7f)
 * @details Extracts bits 25-31 of the instruction, used to further
 *          specify the instruction type
 */
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    uint32_t funct7 = (insn & 0xfe000000) >> (25 - 0);

    return funct7;
}

/**
 * @brief Gets the I-type immediate value from an instruction
 * @param insn The 32-bit instruction
 * @return The sign-extended 32-bit immediate value
 * @details Extracts bits 20-31 and sign-extends to 32 bits
 */
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    // extracts bits 20-31 and shifts them right to position 0-11
    int32_t imm_i = (insn & 0xfff00000) >> (20 - 0);

    // if bit 31 is 1 (negative number), sign-extend
    if (insn & 0x80000000)
    {
        imm_i |= 0xfffff000; // sign-extend the left
    }

    return imm_i;
}

/**
 * @brief Gets the U-type immediate value from an instruction
 * @param insn The 32-bit instruction
 * @return The 32-bit immediate value
 * @details Extracts bits 12-31 (upper 20 bits of the immediate)
 */
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    int32_t imm_u = (insn & 0xfffff000);

    return imm_u;
}

/**
 * @brief Gets the B-type immediate value from an instruction
 * @param insn The 32-bit instruction
 * @return The sign-extended 32-bit immediate value
 * @details Extracts and reassembles immediate bits from various
 *          positions and sign-extends the result
 */
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    // extracts bit 31 (sign bit) and moves it to position 12
    int32_t imm_b = (insn & 0x80000000) >> (31-12);
    // extracts bits 25-30 and moves them to position 5-10
    imm_b |= (insn & 0x7e000000) >> (25-5);
    // extracts bits 8-11 and moves them to position 1-4
    imm_b |= (insn & 0x00000f00) >> (8-1);
    // extracts bit 7 and movies it to position 11
    imm_b |= (insn & 0x00000080) << (11-7);

    // if sign bit is 1, sign-extend
    if (insn & 0x80000000)
    {
        imm_b |= 0xffffe000; // sign-extend the left
    }

    return imm_b;
}

/**
 * @brief Gets the S-type immediate value from an instruction
 * @param insn The 32-bit instruction
 * @return The sign-extended 32-bit immediate value
 * @details Extracts immediate bits from imm[11:5] and imm[4:0],
 *          combines them, and sign-extends the result
 */
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    // extracts bits 25-31 and move them to position 5-11
    int32_t imm_s = (insn & 0xfe000000) >> (25-5);
    // extracts bits 7-11 and moves them to position 0-4
    imm_s |= (insn & 0x00000f80) >> (7-0);
    
    // if sign bit is 1, sign-extend
    if (insn & 0x80000000)
    {
        imm_s |= 0xfffff000; // sign-extend the left
    }

    return imm_s;
}

/**
 * @brief Gets the J-type immediate value from an instruction
 * @param insn The 32-bit instruction
 * @return The sign-extended 32-bit immediate value
 * @details Extracts immediate bits from various positions, combines
 *          them into a 20-bit signed immediate, and sign-extends
 */
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    // extracts bit 31 (sign bit) and moves it to position 20
    int32_t imm_j = (insn & 0x80000000) >> (31-20);
    // extracts bits 21-30 and moves them to position 1-10
    imm_j |= (insn & 0x7fe00000) >> (21-1);
    // extracts bit 20 and moves it to position 11
    imm_j |= (insn & 0x00100000) >> (20-11);
    // extracts bits 12-19, already in correct position
    imm_j |= (insn & 0x000ff000);

    // if sign bit is 1, sign-extend
    if (insn & 0x80000000)
    {
        imm_j |= 0xffe00000;  // sign-extend the left
    }

    return imm_j;
}

/**
 * @brief Renders an illegal instruction message
 * @param insn The 32-bit instruction
 * @return Error message string
 */
std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
 * @brief Renders a load upper immediate instruction
 * @param insn The 32-bit instruction
 * @return Formatted LUI instruction string
 * @details Formats as: lui rd,immediate
 */
std::string rv32i_decode::render_lui(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    
    std::ostringstream os;
    os << render_mnemonic("lui") << render_reg(rd) << ","
       << to_hex0x20((imm_u >> 12)&0x0fffff);
    return os.str();
}

/**
 * @brief Renders an add upper immediate to pc instruction
 * @param insn The 32-bit instruction
 * @return Formatted AUIPC instruction string
 * @details Formats as: auipc rd,immediate
 */
std::string rv32i_decode::render_auipc(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << render_mnemonic("auipc") << render_reg(rd) << ","
       << to_hex0x20((imm_u >> 12)&0x0fffff);

    return os.str();
}

/**
 * @brief Renders a jump and link instruction
 * @param addr The current instruction address
 * @param insn The 32-bit instruction
 * @return Formatted JAL instruction string
 * @details Formats as: jal rd,target_address
 */
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    uint32_t target = addr + imm_j;

    std::ostringstream os;
    os << render_mnemonic("jal") << render_reg(rd) << "," << to_hex0x32(target);
    return os.str();
}

/**
 * @brief Renders a jump and link register instruction
 * @param insn The 32-bit instruction
 * @return Formatted JALR instruction string
 * @details Formats as: jalr rd,offset(rs1)
 */
std::string rv32i_decode::render_jalr(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    std::ostringstream os;
    os << render_mnemonic("jalr") << render_reg(rd) << "," << render_base_disp(rs1, imm_i);
    return os.str();
}

/**
 * @brief Renders a B-type branch instruction
 * @param addr The current instruction address
 * @param insn The 32-bit instruction
 * @param mnemonic The branch instruction name
 * @return Formatted branch instruction string
 * @details Formats as: beq/bne/etc rs1,rs2,target_address
 */
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    uint32_t target = addr + imm_b;

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs1) << "," 
       << render_reg(rs2) << "," << to_hex0x32(target);
    return os.str();
}

/**
 * @brief Renders a load instruction
 * @param insn The 32-bit instruction
 * @param mnemonic The load instruction type (lb/lh/lw/etc)
 * @return Formatted load instruction string
 * @details Formats as: lb/lh/lw/etc rd,offset(rs1)
 */
std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_base_disp(rs1, imm_i);
    return os.str();
}

/**
 * @brief Renders a store instruction
 * @param insn The 32-bit instruction
 * @param mnemonic The store instruction type (sb/sh/sw)
 * @return Formatted store instruction string
 * @details Formats as: sb/sh/sw rs2,offset(rs1)
 */
std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs2) << "," << render_base_disp(rs1, imm_s);
    return os.str();
}

/**
 * @brief Renders an I-type ALU instruction
 * @param insn The 32-bit instruction
 * @param mnemonic The ALU operation (addi/slti/etc)
 * @param imm_i The immediate value
 * @return Formatted ALU instruction string
 * @details Formats as: addi/slti/etc rd,rs1,immediate
 */
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," 
       << render_reg(rs1) << "," << imm_i;
    return os.str();
}

/**
 * @brief Renders an R-type register instruction
 * @param insn The 32-bit instruction
 * @param mnemonic The operation type (add/sub/etc)
 * @return Formatted R-type instruction string
 * @details Formats as: add/sub/etc rd,rs1,rs2
 */
std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
       << render_reg(rs1) << "," << render_reg(rs2);
    return os.str();
}

/**
 * @brief Renders an ECALL instruction
 * @param insn The 32-bit instruction
 * @return The string "ecall" without padding
 * @details Uses render_mnemonic to format without padding
 */
std::string rv32i_decode::render_ecall(uint32_t insn)
{
    return render_mnemonic("ecall");
}

/**
 * @brief Renders an EBREAK instruction
 * @param insn The 32-bit instruction
 * @return The string "ebreak" without padding
 * @details Uses render_mnemonic to format without padding
 */
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
    return render_mnemonic("ebreak");
}

/**
 * @brief Renders a CSR instruction with register operand
 * @param insn The 32-bit instruction
 * @param mnemonic The CSR operation type
 * @return Formatted CSR instruction string
 * @details Formats as: csrrw/csrrs/csrrc rd,csr,rs1
 *          Converts CSR number to hex format
 */
std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t csr = get_imm_i(static_cast<uint32_t>(insn));

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
       << to_hex0x12(csr) << "," << render_reg(rs1);
    return os.str();
}

/**
 * @brief Renders a CSR instruction with immediate operand
 * @param insn The 32-bit instruction
 * @param mnemonic The CSR operation type
 * @return Formatted CSR instruction string
 * @details Formats as: csrrwi/csrrsi/csrrci rd,csr,zimm
 *          Uses rs1 field as zero-extended immediate
 */
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t zimm = get_rs1(insn);
    uint32_t csr = get_imm_i(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," 
       << to_hex0x12(csr) << "," << zimm;
    return os.str();
}

/**
 * @brief Renders a register number as a string
 * @param r The register number (0-31)
 * @return Formatted register string (x0-x31)
 * @details The only function that should be used to format register names
 *          Formats as: x followed by register number
 */
std::string rv32i_decode::render_reg(int r)
{
    std::ostringstream os;
    os << "x" << r;
    return os.str();
}

/**
 * @brief Renders a base-displacement address
 * @param base The base register number
 * @param disp The displacement value
 * @return Formatted base-displacement string
 * @details The only function that should be used to format base-displacement
 *          Formats as: displacement(register) with decimal displacement
 */
std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    std::ostringstream os;
    os << disp << "(" << render_reg(base) << ")";
    return os.str();
}

/**
 * @brief Renders an instruction mnemonic with optional padding
 * @param m The mnemonic string
 * @return Formatted mnemonic string
 * @details Returns unpadded string for ecall/ebreak
 *          Otherwise adds right padding to mnemonic_width characters
 */
std::string rv32i_decode::render_mnemonic(const std::string &m)
{
    if (m == "ecall" || m == "ebreak")
        return m;

    std::ostringstream os;
    os << std::setw(8) << std::left << m;
    return os.str();
}