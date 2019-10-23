/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
	uint32_t pc = CURRENT_STATE.PC;
	instruction inst = *get_inst_info(pc);

	short op;
	unsigned char rs, rt, rd, shamt;
	short imm;
	uint32_t t;

	CURRENT_STATE.PC += 4;
	op = inst.opcode;
	if (op == 0){ // R-type
		rs = inst.r_t.r_i.rs;
		rt = inst.r_t.r_i.rt;
		rd = inst.r_t.r_i.r_i.r.rd;
		shamt = inst.r_t.r_i.r_i.r.shamt;

		switch (inst.func_code){
			case 0: //SLL
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
				break;
			case 2: //SRL
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
				break;
			case 8: //JR
				CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
				break;
			case 33: //ADDU
			    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
			    break;
			case 35: //SUBU
			    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
			    break;
			case 36: //AND
			    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
			    break;
			case 37: // OR
			    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
			    break;
			case 39: // nor
			    CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
			    break;
			case 43: // SLTU
			    CURRENT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])? 1 : 0;
			    break;
		}
	}else if (op == 2){ // j
		t = inst.r_t.target;
		CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (t << 2);
	}else if (op == 3){ // jal
		t = inst.r_t.target;
		CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
		CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (t << 2);
	}else{ // I-type
		rs = inst.r_t.r_i.rs;
		rt = inst.r_t.r_i.rt;
		imm = inst.r_t.r_i.r_i.imm;
		
		switch (op){
			case 4: //BEQ
				if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
					CURRENT_STATE.PC += (uint32_t) (imm << 2);
				break;
			case 5: //BNE
				if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
					CURRENT_STATE.PC += (uint32_t) (imm << 2);
				break;
			case 9: //ADDIU
				CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + (uint32_t) imm;
				break;
			case 11: //SLTIU
				CURRENT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < (uint32_t) imm)? 1 : 0;
				break;
			case 12: //ANDI
				CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (uint32_t) imm;
				break;
			case 13: //ORI
				CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (uint32_t) imm;
				break;
			case 15: //LUI
				CURRENT_STATE.REGS[rt] = (uint32_t) imm << 16;
				break;
			case 35: //LW
				CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + (uint32_t) imm);
				break;
			case 43: //SW
				mem_write_32(CURRENT_STATE.REGS[rs] + (uint32_t) imm, CURRENT_STATE.REGS[rt]);
				break;
		}
	}


    if ((CURRENT_STATE.PC - MEM_TEXT_START) / 4 >= NUM_INST) {
        RUN_BIT = FALSE;
    }
}
