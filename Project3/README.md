# Project 3. MIPS Pipelined Simulator
Skeleton developed by CMU,
modified for KAIST CS311 purpose by THKIM, BKKIM and SHJEON.

## Instructions
There are three files you may modify: `util.h`, `run.h`, and `run.c`.

### 1. util.h

We have setup the basic CPU\_State that is sufficient to implement the project.
However, you may decide to add more variables, and modify/remove any misleading variables.

### 2. run.h

You may add any additional functions that will be called by your implementation of `process_instruction()`.
In fact, we encourage you to split your implementation of `process_instruction()` into many other helping functions.
You may decide to have functions for each stages of the pipeline.
Function(s) to handle flushes (adding bubbles into the pipeline), etc.

### 3. run.c

**Implement** the following function:

    void process_instruction()

The `process_instruction()` function is used by the `cycle()` function to simulate a `cycle` of the pipelined simulator.
Each `cycle()` the pipeline will advance to the next instruction (if there are no stalls/hazards, etc.).
Your internal register, memory, and pipeline register state should be updated according to the instruction
that is being executed at each stage.



### Explain Pipeline Register States




typedef struct ID_EX_Control {   // Pipeline control signal in ID_EX 

	unsigned char MemtoReg;
	unsigned char RegWrite;
	unsigned char Branch;
	unsigned char MemRead;
	unsigned char MemWrite;
	unsigned char RegDst;
	unsigned char ALUOp;
	unsigned char ALUSrc;
	unsigned char ALUCont;  // ALU control input

} ID_EX_cont;

typedef struct EX_MEM_Control {  // Pipeline control signal in EX_MEM 

	unsigned char MemtoReg;
	unsigned char RegWrite;
	unsigned char Branch;
	unsigned char MemRead;
	unsigned char MemWrite;

} EX_MEM_cont;

typedef struct MEM_WB_Control {  // Pipeline control signal in MEM_WB 
	
	unsigned char MemtoReg;
	unsigned char RegWrite;

} MEM_WB_cont;


typedef struct CPU_State_Struct {

	uint32_t PC;			/* program counter for the IF stage*/
	uint32_t REGS[MIPS_REGS];	/* register file */
	
	uint32_t PIPE[PIPE_STAGE];	/* PC being executed at each stage*/
	uint32_t PIPE_STALL[PIPE_STAGE]; /* whether each stage stalled or not */ 
	
	//IF_ID_latch
	instruction* IF_ID_INST;           // Pointer of instruction passed from IF stage to ID stage
	uint32_t IF_ID_NPC;                // Next PC value passed from IF stage to ID stage

	//ID_EX_latch    
	uint32_t ID_EX_NPC;                // Next PC value passed from ID stage to EX stage
	uint32_t ID_EX_REG1;               // Read data 1 value passed from ID stage to EX stage
	uint32_t ID_EX_REG2;               // Read data 2 value passed from ID stage to EX stage
	

	uint32_t ID_EX_IMM;     // 16-bit to 32-bit sign-extend imm value (use when I instruction) passed from ID stage to EX stage
	unsigned char ID_EX_rs;            // Read register 1 passed from ID stage to EX stage
	unsigned char ID_EX_rt;       	   // Read register 2 passed from ID stage to EX stage
	unsigned char ID_EX_rd;	 		   // Register number of rd (use when R instruction) passed from ID stage to EX stage
	unsigned char ID_EX_shamt;         // Shamt value (use when sll, srl) passed from ID stage to EX stage

	ID_EX_cont ID_EX;                  // Pipeline control signal in ID_EX
	
	//EX_MEM_latch
	uint32_t EX_MEM_NPC;               // Next PC value passed from EX stage to MEM stage
	uint32_t EX_MEM_ALU_OUT;           // Output of ALU in EX stage passed from EX stage to MEM stage
	uint32_t EX_MEM_W_VALUE;           // Value to write in memory(use when sw) passed from EX stage to MEM stage
	uint32_t EX_MEM_BR_TARGET;         // Pc value when branch succeeded (use when branch instruction) passed from EX stage to MEM stage
	unsigned char EX_MEM_write_Reg;    // Write register number passed from EX stage to MEM stage
	unsigned char EX_MEM_W_VALUE_reg;  // The register number where the value to write in memory was stored (need
	                                   // when MEM/WB-to-MEM forwarding) passed from EX stage to MEM stage

	EX_MEM_cont EX_MEM;                // Pipeline control signal in EX_MEM 

	//MEM_WB_latch
	uint32_t MEM_WB_NPC;               // Next PC value passed from MEM stage to WB stage
	uint32_t MEM_WB_ALU_OUT;           // Output of ALU in EX stage passed from MEM stage to WB stage
	uint32_t MEM_WB_MEM_OUT;           // Value to write in memory(use when sw) passed from MEM stage to WB stage
	unsigned char MEM_WB_write_Reg;    // Write register number passed from MEM stage to WB stage

	MEM_WB_cont MEM_WB;                // Pipeline control signal in MEM_WB 

	//Forwarding
	unsigned char EX_MEM_FORWARD_REG;  // The register number to be forwarded from EX_MEM
	unsigned char MEM_WB_FORWARD_REG;  // The register number to be forwarded from MEM_WB
	uint32_t EX_MEM_FORWARD_VALUE;     // The value to be forwarded from EX_MEM
	uint32_t MEM_WB_FORWARD_VALUE;     // The value to be forwarded from EX_MEM

} CPU_State;