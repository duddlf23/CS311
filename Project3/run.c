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
instruction* get_inst_info(uint32_t pc) { 
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

	
	/** Your implementation here */
	uint32_t count = 0;
	
	// WB stage
	CURRENT_STATE.PIPE[WB_STAGE] = CURRENT_STATE.PIPE[MEM_STAGE];

	
	if (CURRENT_STATE.PIPE[WB_STAGE]){
		if (CURRENT_STATE.MEM_WB.RegWrite){
			unsigned char reg = CURRENT_STATE.MEM_WB_write_Reg;
			if(CURRENT_STATE.MEM_WB.MemtoReg){
				CURRENT_STATE.REGS[reg] = CURRENT_STATE.MEM_WB_MEM_OUT;
			}else{
				CURRENT_STATE.REGS[reg] = CURRENT_STATE.MEM_WB_ALU_OUT;
			}
		}
		INSTRUCTION_COUNT++;
		
		count += 1;
	}

	// MEM stage
	unsigned char branch = 0;

	CURRENT_STATE.PIPE[MEM_STAGE] = CURRENT_STATE.PIPE[EX_STAGE];
	
	if (CURRENT_STATE.PIPE[MEM_STAGE]){
		if (CURRENT_STATE.EX_MEM.MemWrite){
			mem_write_32(CURRENT_STATE.EX_MEM_ALU_OUT, CURRENT_STATE.EX_MEM_W_VALUE);
		}
		if (CURRENT_STATE.EX_MEM.MemRead){
			CURRENT_STATE.MEM_WB_MEM_OUT = mem_read_32(CURRENT_STATE.EX_MEM_ALU_OUT);
		}
		if (CURRENT_STATE.EX_MEM.Branch && CURRENT_STATE.EX_MEM_ALU_OUT){
			branch = 1;
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;
		}
		CURRENT_STATE.MEM_WB_NPC = CURRENT_STATE.EX_MEM_NPC;
		CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
		CURRENT_STATE.MEM_WB_write_Reg = CURRENT_STATE.EX_MEM_write_Reg;
		CURRENT_STATE.MEM_WB.MemtoReg = CURRENT_STATE.EX_MEM.MemtoReg;
		CURRENT_STATE.MEM_WB.RegWrite = CURRENT_STATE.EX_MEM.RegWrite;
		
		count += 2;
	}

	if (branch){
		CURRENT_STATE.PIPE[EX_STAGE] = 0;
		CURRENT_STATE.PIPE[ID_STAGE] = 0;
		CURRENT_STATE.PIPE[IF_STAGE] = 0;
		return;
	}	


	// EX stage
	if(CURRENT_STATE.PIPE_STALL[ID_STAGE]){
		CURRENT_STATE.PIPE[EX_STAGE] = 0;
	}else{
		CURRENT_STATE.PIPE[EX_STAGE] = CURRENT_STATE.PIPE[ID_STAGE];
	}

	if (CURRENT_STATE.PIPE[EX_STAGE]){
		// ALU operation
		uint32_t result = 0, data1, data2;
		switch (CURRENT_STATE.ID_EX.ALUSrc){
			case 0:
				data1 = CURRENT_STATE.ID_EX_REG1;
				data2 = CURRENT_STATE.ID_EX_REG2;
				break;
			case 1:
				data1 = CURRENT_STATE.ID_EX_REG1;
				data2 = CURRENT_STATE.ID_EX_IMM;
				break;
			case 2:
				data1 = CURRENT_STATE.ID_EX_REG2;
				data2 = (uint32_t) CURRENT_STATE.ID_EX_shamt;
				break;
			case 3:
				CURRENT_STATE.PIPE[IF_STAGE] = 0;
				break;
		}
		switch (CURRENT_STATE.ID_EX.ALUCont){
			case 0: // AND
				result = data1 & data2;
				break;
			case 1: // OR
				result = data1 | data2;
				break;
			case 2: // ADD
				result = data1 + data2;
				break;
			case 3: // SUB
				result = data1 - data2;
				break;
			case 4: // NOR
				result = ~(data1 | data2);
				break;
			case 5: // SLL
				result = data1 << data2;
				break;
			case 6: // SRL
				result = data1 >> data2;
				break;
			case 7: // SLT
				result = (data1 < data2)? 1 : 0;
				break;
			case 8: // LUI
				result = data2 << 16;
				break;
			case 9: // BEQ
				result = (data1 == data2)? 1 : 0;
				break;
			case 10: // BNE
				result = (data1 != data2)? 1 : 0;
				break;
			case 12: // JAL
				result = CURRENT_STATE.ID_EX_REG2;
				break;

		}

		CURRENT_STATE.EX_MEM_ALU_OUT = result;

		// write reg
		if (CURRENT_STATE.ID_EX.RegDst){
			CURRENT_STATE.EX_MEM_write_Reg = CURRENT_STATE.ID_EX_rd;
		}else{
			CURRENT_STATE.EX_MEM_write_Reg = CURRENT_STATE.ID_EX_rt;
		}

		// write data
		if (CURRENT_STATE.ID_EX.MemWrite){
			CURRENT_STATE.EX_MEM_W_VALUE = CURRENT_STATE.ID_EX_REG2;
			CURRENT_STATE.EX_MEM_W_VALUE_reg = CURRENT_STATE.ID_EX_rt;
		}

		CURRENT_STATE.EX_MEM_NPC = CURRENT_STATE.ID_EX_NPC;
		
		if (CURRENT_STATE.ID_EX.Branch){
			CURRENT_STATE.EX_MEM_BR_TARGET = CURRENT_STATE.ID_EX_NPC + (CURRENT_STATE.ID_EX_IMM << 2);
		}
		// control
		CURRENT_STATE.EX_MEM.MemtoReg = CURRENT_STATE.ID_EX.MemtoReg;
		CURRENT_STATE.EX_MEM.RegWrite = CURRENT_STATE.ID_EX.RegWrite;
		CURRENT_STATE.EX_MEM.Branch = CURRENT_STATE.ID_EX.Branch;
		CURRENT_STATE.EX_MEM.MemRead = CURRENT_STATE.ID_EX.MemRead;
		CURRENT_STATE.EX_MEM.MemWrite = CURRENT_STATE.ID_EX.MemWrite;

		count += 3;

	}


	// ID stage
	short op, func;
	uint32_t t;

	unsigned char MemtoReg = 0, RegWrite = 0, Branch = 0, MemRead = 0, MemWrite = 0, RegDst = 0;
	unsigned char ALUSrc , ALUCont;
	instruction inst;

	unsigned char jump = 0;	
	uint32_t jump_target;

	if(CURRENT_STATE.PIPE_STALL[ID_STAGE]){
		CURRENT_STATE.PIPE_STALL[ID_STAGE] = 0;
	}else{
		if(CURRENT_STATE.PIPE_STALL[IF_STAGE]){
			CURRENT_STATE.PIPE[EX_STAGE] = 0;
		}else{
			CURRENT_STATE.PIPE[ID_STAGE] = CURRENT_STATE.PIPE[IF_STAGE];
		}
		if (CURRENT_STATE.PIPE[ID_STAGE]){
			inst = *CURRENT_STATE.IF_ID_INST;
			op = inst.opcode;
			func = inst.func_code;
			CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.IF_ID_NPC;
			CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.REGS[inst.r_t.r_i.rs];
			CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.REGS[inst.r_t.r_i.rt];

			CURRENT_STATE.ID_EX_rs = inst.r_t.r_i.rs;
			CURRENT_STATE.ID_EX_rt = inst.r_t.r_i.rt;
			CURRENT_STATE.ID_EX_rd = inst.r_t.r_i.r_i.r.rd;
			CURRENT_STATE.ID_EX_IMM = (uint32_t) inst.r_t.r_i.r_i.imm;
			CURRENT_STATE.ID_EX_shamt = inst.r_t.r_i.r_i.r.shamt;
			t = inst.r_t.target;

			if (op == 0){ // R-type
				RegWrite = 1, RegDst = 1;
				switch (func){
					case 0: //SLL
						ALUSrc = 2, ALUCont = 5;
						break;
					case 2: //SRL
						ALUSrc = 2, ALUCont = 6;
						break;
					case 8: //JR
						RegWrite = 0, ALUSrc = 3, ALUCont = 11;
						jump = 1;
						jump_target = CURRENT_STATE.ID_EX_REG1;
						break;
					case 33: //ADDU
						ALUSrc = 0, ALUCont = 2;
						break;
					case 35: //SUBU
						ALUSrc = 0, ALUCont = 3;
						break;
					case 36: //AND
						ALUSrc = 0, ALUCont = 0;
						break;
					case 37: //OR
						ALUSrc = 0, ALUCont = 1;
						break;
					case 39: //NOR
						ALUSrc = 0, ALUCont = 4;
						break;
					case 43: //SLTU
						ALUSrc = 0, ALUCont = 7;
				}
			}else if (op == 2){ // J
				ALUSrc = 3, ALUCont = 11;
				jump = 1;
				jump_target = (CURRENT_STATE.IF_ID_NPC & 0xf0000000) | (t << 2);
			}else if (op == 3){ // JAL
				RegWrite = 1, RegDst = 1;
				CURRENT_STATE.ID_EX_rd = 31;
				CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.IF_ID_NPC + 4;
				ALUSrc = 3, ALUCont = 12;
				jump = 1;
				jump_target = (CURRENT_STATE.IF_ID_NPC & 0xf0000000) | (t << 2);
			}else{ // I-type
				switch (op){
					case 4: //BEQ
						Branch = 1, ALUSrc = 0, ALUCont = 9;
						break;
					case 5: //BNE
						Branch = 1, ALUSrc = 0, ALUCont = 10;
						break;
					case 9: //ADDIU
						ALUSrc = 1, ALUCont = 2, RegWrite = 1;
						break;
					case 11: //SLTIU
						ALUSrc = 1, ALUCont = 7, RegWrite = 1;
						break;
					case 12: //ANDI
						ALUSrc = 1, ALUCont = 0, RegWrite = 1;
						break;
					case 13: //ORI
						ALUSrc = 1, ALUCont = 1, RegWrite = 1;
						break;
					case 15: //LUI
						ALUSrc = 1, ALUCont = 8, RegWrite = 1;
						break;
					case 35: //LW
						ALUSrc = 1, ALUCont = 2, RegWrite = 1, MemRead = 1, MemtoReg = 1;
						break;
					case 43: //SW
						ALUSrc = 1, ALUCont = 2, MemWrite = 1;
						break;
				}
			}

			//control cpoy
			CURRENT_STATE.ID_EX.MemtoReg = MemtoReg;
			CURRENT_STATE.ID_EX.RegWrite = RegWrite;
			CURRENT_STATE.ID_EX.Branch = Branch;
			CURRENT_STATE.ID_EX.MemRead = MemRead;
			CURRENT_STATE.ID_EX.MemWrite = MemWrite;
			CURRENT_STATE.ID_EX.RegDst = RegDst;
			CURRENT_STATE.ID_EX.ALUSrc = ALUSrc;
			CURRENT_STATE.ID_EX.ALUCont = ALUCont;
			

			count += 4;
		}
	}

	uint32_t pc = CURRENT_STATE.PC;

	if (CURRENT_STATE.PIPE_STALL[IF_STAGE]){
		CURRENT_STATE.PIPE_STALL[IF_STAGE] = 0;
		CURRENT_STATE.PC += 4;
	}else{

		if ((pc - MEM_TEXT_START) / 4 >= NUM_INST){
			CURRENT_STATE.PIPE[IF_STAGE] = 0;
			if (count <= 1){
				RUN_BIT = FALSE;
			}
		}else{
			CURRENT_STATE.IF_ID_INST = get_inst_info(pc);
			inst = *CURRENT_STATE.IF_ID_INST;
			CURRENT_STATE.PIPE[IF_STAGE] = pc;
			CURRENT_STATE.PC += 4;
			CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PC;
		}

		if (jump){
			CURRENT_STATE.PC = jump_target;
		}
	}

	// forwarding
	CURRENT_STATE.MEM_WB_FORWARD_REG = 0;
	if (CURRENT_STATE.PIPE[MEM_STAGE]){
		if (CURRENT_STATE.MEM_WB.RegWrite){
			if (CURRENT_STATE.MEM_WB_write_Reg != 0){
				if (CURRENT_STATE.MEM_WB_write_Reg == CURRENT_STATE.ID_EX_rs || CURRENT_STATE.MEM_WB_write_Reg == CURRENT_STATE.ID_EX_rt){
					CURRENT_STATE.MEM_WB_FORWARD_REG = CURRENT_STATE.MEM_WB_write_Reg;
					if (CURRENT_STATE.MEM_WB.MemtoReg){
						CURRENT_STATE.MEM_WB_FORWARD_VALUE = CURRENT_STATE.MEM_WB_MEM_OUT;
					}else{
						CURRENT_STATE.MEM_WB_FORWARD_VALUE = CURRENT_STATE.MEM_WB_ALU_OUT;
					}
				} 
			}

			if (CURRENT_STATE.EX_MEM.MemWrite){
				if (CURRENT_STATE.EX_MEM_W_VALUE_reg == CURRENT_STATE.MEM_WB_write_Reg){
					CURRENT_STATE.EX_MEM_W_VALUE = CURRENT_STATE.MEM_WB_MEM_OUT;
				}

			}
		}
	}
	CURRENT_STATE.EX_MEM_FORWARD_REG = 0;
	if (CURRENT_STATE.PIPE[EX_STAGE]){
		if (CURRENT_STATE.EX_MEM.RegWrite){
			if (CURRENT_STATE.EX_MEM_write_Reg != 0){
				if (CURRENT_STATE.EX_MEM_write_Reg == CURRENT_STATE.ID_EX_rs || CURRENT_STATE.EX_MEM_write_Reg == CURRENT_STATE.ID_EX_rt){
					if (CURRENT_STATE.EX_MEM.MemRead){
						CURRENT_STATE.PIPE_STALL[IF_STAGE] = 1;
						CURRENT_STATE.PIPE_STALL[ID_STAGE] = 1;
						CURRENT_STATE.PC -= 4;
					}else{
						CURRENT_STATE.EX_MEM_FORWARD_REG = CURRENT_STATE.EX_MEM_write_Reg;
						CURRENT_STATE.EX_MEM_FORWARD_VALUE = CURRENT_STATE.EX_MEM_ALU_OUT;
					}
				}
			}
		}
	}
	if (CURRENT_STATE.MEM_WB_FORWARD_REG != 0){
		if (CURRENT_STATE.MEM_WB_FORWARD_REG == CURRENT_STATE.ID_EX_rs){
			CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.MEM_WB_FORWARD_VALUE;
		}
		if (CURRENT_STATE.MEM_WB_FORWARD_REG == CURRENT_STATE.ID_EX_rt){
			CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.MEM_WB_FORWARD_VALUE;
		}
	}
	if (CURRENT_STATE.EX_MEM_FORWARD_REG != 0){
		if (CURRENT_STATE.EX_MEM_FORWARD_REG == CURRENT_STATE.ID_EX_rs){
			CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.EX_MEM_FORWARD_VALUE;
		}
		if (CURRENT_STATE.EX_MEM_FORWARD_REG == CURRENT_STATE.ID_EX_rt){
			CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.EX_MEM_FORWARD_VALUE;
		}
	}
}
