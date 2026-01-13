#include "rv32i_hart.h"

// Reset the rv32i object and the registerfile (see doc).
void rv32i_hart::reset()
{
    pc = 0;
    insn_counter = 0;
    halt = false;
    halt_reason = "none";

    regs.reset();
}

// see documentation
void rv32i_hart::dump(const std::string &hdr) const
{
    regs.dump(hdr);

    if (hdr != "")
    {
        std::cout << hdr;
    }

    // if this doesn't work, do setw(4) and "pc "
    std::cout << std::setw(3) <<  "pc" << " " << hex::to_hex32(pc);
}

// see documentation
void rv32i_hart::tick(const std::string &hdr)
{
    if (!halt)
    {
        if (show_registers)
        {
            dump(hdr);
        }

        if (!(pc % 4 == 0))
        {
            halt = true;
            halt_reason = "PC alignment error";
            return;
        }

        insn_counter++;
        uint32_t insn = mem.get32(pc);

        if (show_instructions)
        {
            std::cout << hdr << to_hex32(pc) << ": " << to_hex32(insn);
            exec(insn, &std::cout);
        }
        else
        {
            exec(insn, nullptr);
        }
    }
}

// similar to decode(...), see doc.
void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
    switch(get_opcode(insn))
    {
        default:                            exec_illegal_insn(insn, pos); return;
        case opcode_lui:                    exec_lui(insn, pos); return;
        case opcode_auipc:                  exec_auipc(insn, pos); return;
        case opcode_jal:                    exec_jal(insn, pos); return;
        case opcode_jalr:                   exec_jalr(insn, pos); return;
        case opcode_btype:
            switch(get_funct3(insn))
            {
                default:                    exec_illegal_insn(insn, pos); return;
                case funct3_beq:            exec_beq(insn, pos); return;
                case funct3_bne:            exec_bne(insn, pos); return;
                case funct3_blt:            exec_blt(insn, pos); return;
                case funct3_bge:            exec_bge(insn, pos); return;
                case funct3_bltu:           exec_bltu(insn, pos); return;
                case funct3_bgeu:           exec_bgeu(insn, pos); return;
            }
            assert(0 && "unrecognized funct3");
        case opcode_load_imm:
            switch(get_funct3(insn))
            {
                default:                    exec_illegal_insn(insn, pos); return;
                case funct3_lb:             exec_lb(insn, pos); return; 
                case funct3_lh:             exec_lh(insn, pos); return; 
                case funct3_lw:             exec_lw(insn, pos); return; 
                case funct3_lbu:            exec_lbu(insn, pos); return; 
                case funct3_lhu:            exec_lhu(insn, pos); return; 
            }
            assert(0 && "unrecognized funct3");
        case opcode_stype:
            switch(get_funct3(insn))
            {
                default:                    exec_illegal_insn(insn, pos); return;
                case funct3_sb:             exec_sb(insn, pos); return;
                case funct3_sh:             exec_sh(insn, pos); return;
                case funct3_sw:             exec_sw(insn, pos); return;
            }
            assert(0 && "unrecognized funct3");
        case opcode_alu_imm:
            switch(get_funct3(insn))
            {
                default:                    exec_illegal_insn(insn, pos); return;
                case funct3_add:            exec_addi(insn, pos); return;
                case funct3_slt:            exec_slti(insn, pos); return;
                case funct3_sltu:           exec_sltiu(insn, pos); return;
                case funct3_xor:            exec_xori(insn, pos); return;
                case funct3_or:             exec_ori(insn, pos); return;
                case funct3_and:            exec_andi(insn, pos); return;
                case funct3_sll:            exec_slli(insn, pos); return;
                case funct3_srx:   
                    switch(get_funct7(insn))
                    {
                        default:            exec_illegal_insn(insn, pos); return;
                        case funct7_srl:    exec_srli(insn, pos); return;
                        case funct7_sra:    exec_srai(insn, pos); return;
                    }         
                    assert(0 && "unrecognized funct7");
            }
            assert(0 && "unrecognized funct3");
        case opcode_rtype:
            switch(get_funct3(insn))
            {
                default:                    exec_illegal_insn(insn, pos); return;
                case funct3_add:
                    switch(get_funct7(insn))
                    {
                        default:            exec_illegal_insn(insn, pos); return;
                        case funct7_add:    exec_add(insn, pos); return;
                        case funct7_sub:    exec_sub(insn, pos); return;
                    }
                    assert(0 && "unrecognized funct7");
                case funct3_sll:            exec_sll(insn, pos); return;
                case funct3_slt:            exec_slt(insn, pos); return;
                case funct3_sltu:           exec_sltu(insn, pos); return;
                case funct3_xor:            exec_xor(insn, pos); return;
                case funct3_srx:
                    switch(get_funct7(insn))
                    {
                        default:            exec_illegal_insn(insn, pos); return;
                        case funct7_srl:    exec_srl(insn, pos); return;
                        case funct7_sra:    exec_sra(insn, pos); return;
                    }
                    assert(0 && "unrecognized funct7");
                case funct3_or:             exec_or(insn, pos); return;
                case funct3_and:            exec_and(insn, pos); return;
            }
            assert(0 && "unrecognized funct3");
        case opcode_system:
            switch(insn)
            {
                case insn_ecall:            exec_ecall(insn, pos); return;
                case insn_ebreak:           exec_ebreak(insn, pos); return;
                default:
                    switch(get_funct3(insn))
                    {
                        default:            exec_illegal_insn(insn, pos); return;
                        case funct3_csrrw:  exec_csrrw(insn, pos); return;
                        case funct3_csrrs:  exec_csrrs(insn, pos); return;
                        case funct3_csrrc:  exec_csrrc(insn, pos); return;
                        case funct3_csrrwi: exec_csrrwi(insn, pos); return;
                        case funct3_csrrsi: exec_csrrsi(insn, pos); return;
                        case funct3_csrrci: exec_csrrci(insn, pos); return;
                    }
                    assert(0 && "unrecognized funct3");
            }
            assert(0 && "unrecognized insn");
    }
    assert(0 && "unrecognized opcode");
}

// Set the halt ag and, if the ostream* parameter is not nullptr then use 
// render_illegal_insn() to render the proper error message by writing it to
// the pos output stream (see doc)
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        *pos << render_illegal_insn(insn);
    }

    halt = true;
    halt_reason = "Illegal instruction";
}

void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    if (pos)
    {
        std::string s = render_lui(insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(imm_u);
    }

    regs.set(rd, imm_u);
    pc += 4;
}

void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    uint32_t val = pc + imm_u;

    if (pos)
    {
        std::string s = render_auipc(insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc) << " + "
             << hex::to_hex0x32(imm_u);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);

    uint32_t val1 = pc + 4;
    uint32_t val2 = pc + imm_j;

    if (pos)
    {
        std::string s = render_jal(pc, insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(val1) << ",  "
             << "pc = " << hex::to_hex0x32(pc) << " + "<< hex::to_hex0x32(imm_j) 
             << " = " << hex::to_hex0x32(val2);
    }

    regs.set(rd, val1);
    pc = val2;
}

void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    uint32_t val1 = pc + 4;
    uint32_t val2 = (rs1_val + imm_i) & ~1;

    if (pos)
    {
        std::string s = render_jalr(insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(val1) << ",  "
             << "pc = (" << hex::to_hex0x32(rs1_val) << " + "<< hex::to_hex0x32(imm_i) 
             << ") & " << hex::to_hex0x32(~1) << " = " << hex::to_hex0x32(val2);
    }

    regs.set(rd, val1);
    pc = val2;
}

void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val == rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "beq");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " == "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val != rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bne");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " != "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val < rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "blt");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " < "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val >= rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bge");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " >= "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val < rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bltu");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " < "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t rs2_val = regs.get(rs2);
    int32_t imm_b = get_imm_b(insn);

    uint32_t val = pc + ((rs1_val >= rs2_val) ? imm_b : 4);

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bgeu");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(rs1_val) << " >= "<< hex::to_hex0x32(rs2_val) 
             << " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }

    pc = val;
}

void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = mem.get8_sx(rs1_val + imm_i);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lb");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << "sx(m8(" << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = mem.get16_sx(rs1_val + imm_i);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lh");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << "sx(m16(" << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = mem.get32_sx(rs1_val + imm_i);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lw");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << "sx(m32(" << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = mem.get8(rs1_val + imm_i);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lw");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << "sx(m32(" << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    uint32_t val = mem.get16(rs1_val + imm_i);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lhu");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << "zx(m16(" << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_s = get_imm_s(insn);

    uint32_t addr = rs1_val + imm_s;
    uint8_t val = rs2_val & 0x000000ff;

    if (pos)
    {
        std::string s = render_stype(insn, "sb");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m8(" << hex::to_hex0x32(rs1_val) << " + " << hex::to_hex0x32(imm_s) 
             << ") = " << hex::to_hex0x32(val);
    }

    mem.set8(addr, val);
    pc += 4;
}

void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_s = get_imm_s(insn);

    uint32_t addr = rs1_val + imm_s;
    uint16_t val = rs2_val & 0x0000ffff;

    if (pos)
    {
        std::string s = render_stype(insn, "sh");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m16(" << hex::to_hex0x32(rs1_val) << " + " << hex::to_hex0x32(imm_s) 
             << ") = " << hex::to_hex0x32(val);
    }

    mem.set16(addr, val);
    pc += 4;
}

void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);
    int32_t imm_s = get_imm_s(insn);

    uint32_t addr = rs1_val + imm_s;
    uint32_t val = rs2_val;

    if (pos)
    {
        std::string s = render_stype(insn, "sw");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m32(" << hex::to_hex0x32(rs1_val) << " + " << hex::to_hex0x32(imm_s) 
             << ") = " << hex::to_hex0x32(val);
    }

    mem.set32(addr, val);
    pc += 4;
}

void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = rs1_val + imm_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "addi", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " + " 
             << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = (rs1_val < imm_i) ? 1 : 0;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "slti", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1_val) << " < "
             << hex::to_hex0x32(imm_i) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t imm_i = get_imm_i(insn);

    int32_t val = (rs1_val < imm_i) ? 1 : 0;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "sltiu", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1_val) << " < "
             << hex::to_hex0x32(imm_i) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = rs1_val ^ imm_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "xori", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " ^ "
             << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = rs1_val | imm_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "ori", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " | "
             << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = rs1_val & imm_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "andi", imm_i);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " & "
             << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    uint32_t shamt_i = get_rs2(insn);

    int32_t val = rs1_val << shamt_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "slli", get_imm_i(insn)%XLEN);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " << "
             << hex::to_hex0x32(shamt_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    uint32_t shamt_i = get_rs2(insn);

    uint32_t val = rs1_val >> shamt_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " >> "
             << hex::to_hex0x32(shamt_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t rs1_val = regs.get(rs1);
    uint32_t shamt_i = get_rs2(insn);

    int32_t val = rs1_val >> shamt_i;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " << "
             << hex::to_hex0x32(shamt_i) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val + rs2_val;

    if (pos)
    {
        std::string s = render_rtype(insn, "add");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " + "
             << hex::to_hex0x32(rs2_val) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val - rs2_val;

    if (pos)
    {
        std::string s = render_rtype(insn, "sub");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " - "
             << hex::to_hex0x32(rs2_val) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val << (rs2_val % XLEN);

    if (pos)
    {
        std::string s = render_rtype(insn, "sll");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " << "
             << hex::to_hex0x32(rs2_val % XLEN) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = (rs1_val < rs2_val) ? 1 : 0;

    if (pos)
    {
        std::string s = render_rtype(insn, "slt");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1_val) << " < "
             << hex::to_hex0x32(rs2_val) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t rs2_val = regs.get(rs2);

    int32_t val = (rs1_val < rs2_val) ? 1 : 0;

    if (pos)
    {
        std::string s = render_rtype(insn, "sltu");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1_val) << " < "
             << hex::to_hex0x32(rs2_val) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val ^ rs2_val;

    if (pos)
    {
        std::string s = render_rtype(insn, "xor");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " ^ "
             << hex::to_hex0x32(rs2_val) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1_val = regs.get(rs1);
    uint32_t rs2_val = regs.get(rs2);

    uint32_t val = rs1_val >> (rs2_val % XLEN);

    if (pos)
    {
        std::string s = render_rtype(insn, "srl");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " >> "
             << hex::to_hex0x32(rs2_val % XLEN) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val >> (rs2_val % XLEN);

    if (pos)
    {
        std::string s = render_rtype(insn, "sra");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " << "
             << hex::to_hex0x32(rs2_val % XLEN) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val | rs2_val;

    if (pos)
    {
        std::string s = render_rtype(insn, "or");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " | "
             << hex::to_hex0x32(rs2_val) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1_val = regs.get(rs1);
    int32_t rs2_val = regs.get(rs2);

    int32_t val = rs1_val & rs2_val;

    if (pos)
    {
        std::string s = render_rtype(insn, "and");

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1_val) << " & "
             << hex::to_hex0x32(rs2_val) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_ecall(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ecall(insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT";
    }

    halt = true;
    halt_reason = "ECALL instruction";
}

void rv32i_hart::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ebreak(insn);

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT";
    }

    halt = true;
    halt_reason = "EBREAK instruction";
}

void rv32i_hart::exec_csrrw(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  
    
    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);

    // csr write ignored in simple implementation

    pc += 4;
}

void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  
    
    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrs");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);
    // CSR modification ignored in simple implementation
    pc += 4;
}

void rv32i_hart::exec_csrrc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  // Simple implementation: return 0 for any CSR read
    
    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrc");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);
    // CSR modification ignored in simple implementation
    pc += 4;
}

void rv32i_hart::exec_csrrwi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  // Simple implementation: return 0 for any CSR read
    
    if (pos)
    {
        std::string s = render_csrrxi(insn, "csrrwi");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);
    // CSR write ignored in simple implementation
    pc += 4;
}

void rv32i_hart::exec_csrrsi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  // Simple implementation: return 0 for any CSR read
    
    if (pos)
    {
        std::string s = render_csrrxi(insn, "csrrsi");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);
    // CSR modification ignored in simple implementation
    pc += 4;
}

void rv32i_hart::exec_csrrci(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t old_csr_val = 0;  // Simple implementation: return 0 for any CSR read
    
    if (pos)
    {
        std::string s = render_csrrxi(insn, "csrrci");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(old_csr_val);
    }

    regs.set(rd, old_csr_val);
    // CSR modification ignored in simple implementation
    pc += 4;
}