#ifndef _INC_DISASM_
#define _INC_DISASM_

#if _MSC_VER > 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////
// Instruction Types
#define CODE_DIGIT 1	// /digit; opcode extension
#define CODE_R 2	// /r; ModR/M byte contains both register op and r/m op.
//#define CODE_PRB 4	// +rb; byte register
//#define CODE_PRW 8	// +rw; word register 
//#define CODE_PRD 16	// +rd; dword register
//#define CODE_PI 32	// +i; st(i) register
#define CODE_PREFIX 64		// first prefix
#define CODE_PREFIX_2 128		// sec prefix
//////////////////////////////////////////////////////////////////////////
// Operand Types
#define OP_NULL 0		// no operand
#define OP_CB 1			// cb; a relative byte offset
#define OP_CW 2		// cw; a relative word offset
#define OP_CD 3		// cd; a relative dword offset
#define OP_CP 4		// cp; an absolute far pointer (JMP, CALL, ...)
//#define OP_IB 5		// ib; byte immediate operand
//#define OP_IW 6		// iw; word immediate operand
//#define OP_ID 7		// id; dword immediate operand
#define OP_REL8 8		// byte relative address (-127~128)
#define OP_REL16 9		// word relative address (-32767~32767)
#define OP_REL32 10		// dword relative address (-2147483647~2147483647)
#define OP_PTR_FAR_1616 11	// ptr16:16, far pointer
#define OP_PTR_FAR_1632 12	// ptr16:32, far pointer
#define OP_R8 13		// general-purpose byte register
#define OP_R16 14		// general-purpose word register
#define OP_R32 15		// general-purpose dword register
#define OP_IMM8 16		// immediate byte value
#define OP_IMM16 17		// immediate word value
#define OP_IMM32 18		// immediate dword value
#define OP_RM8 19		// byte general-purpose register or memory (pointer)
#define OP_RM16 20		// word general-purpose register or memory (pointer)
#define OP_RM32 21		// dword general-purpose register or memory (pointer)
#define OP_M 22		// word or dword pointer operand
#define OP_M8 23		// byte pointer operand
#define OP_M16 24		// word pointer operand
#define OP_M32 25		// dword pointer operand
#define OP_M64 26		// qword pointer operand
#define OP_MFAR_1616 27		// m16:16, far pointer
#define OP_MFAR_1632 28		// m16:32, far pointer
#define OP_MPAIR_1632 29	// m16&32, memory operand (size indicated)
#define OP_MPAIR_1616 30	// m16&16, memory operand (size indicated)
#define OP_MPAIR_3232 31		// m32&32, memory operand (size indicated)
#define OP_MOFFS8 32		// byte memory offset
#define OP_MOFFS16 33		// word memory offset
#define OP_MOFFS32 34		// dword memory offset
#define OP_SREG 35		// segment register (es, cs, ss, ds, fs, gs)
#define OP_M32REAL 36		// single floating-point operand
#define OP_M64REAL 37		// double floating-point operand
#define OP_M80REAL 38		// extended-real floating-point operand
#define OP_M16INT 39		// word-integer floating-point operand
#define OP_M32INT 40		// short-integer floating-point operand
#define OP_M64INT 41		// long-integer floating-point operand
#define OP_ST 42		// the top FPU register stack
#define OP_ST_I 43		// the I th FPU register stack
#define OP_MMXREG 43		// MMX Register
#define OP_MMXRM32 44		// low order 32bits of MMX register or 32-bit memory operand
#define OP_MMXRM64 45		// mmx register or 64-bit memory operand
#define OP_CR 46		// debug register CR0-CR7
#define OP_DR 47		// debug register DR0-DR7
#define OP_M1428BYTE 48		// m14/28byte
#define OP_M2BYTE 49
#define OP_M94108BYTE 50		// m94/108byte
#define OP_M80BCD 51			// m80bcd
//////////////////////////////////////////////////////////////////////////
// Register Types

// 1 Byte Registers
#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

// 2 Byte Registers
#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

// 4 Byte Registers
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

// MMX Registers
#define MM0 0
#define MM1 1
#define MM2 2
#define MM3 3
#define MM4 4
#define MM5 5
#define MM6 6
#define MM7 7

// XMM Registers
#define XMM0 0
#define XMM1 1
#define XMM2 2
#define XMM3 3
#define XMM4 4
#define XMM5 5
#define XMM6 6
#define XMM7 7

// CR Registers
#define CR0 1
#define CR2 2
#define CR3 3
#define CR4 4
#define CR5 5
#define CR6 6
#define CR7 7

// DR Registers
#define DR0 0
#define DR1 1
#define DR2 2
#define DR3 3
#define DR4 4
#define DR5 5
#define DR6 6
#define DR7 7

// Effective Address Types
// Mod 0
#define EA_EAX 0		// [eax]
#define EA_ECX 1		// [ecx]
#define EA_EDX 2		// [edx]
#define EA_EBX 3		// [ebx]
#define EA_SIB 4		// SIB Byte required
#define EA_DISP32 5		// disp32
#define EA_ESI 6		// [esi]
#define EA_EDI 7		// [edi]

// Mod 1
#define EA_EAX_DISP8 0		// [eax]+disp8
#define EA_ECX_DISP8 1		// [ecx]+disp8
#define EA_EDX_DISP8 2		// [edx]+disp8
#define EA_EBX_DISP8 3		// [ebx]+disp8
#define EA_EBP_DISP8 5		// [ebp]+disp8
#define EA_ESI_DISP8 6		// [esi]+disp8
#define EA_EDI_DISP8 7		// [edi]+disp8

// Mod 2
#define EA_EAX_DISP32 0		// [eax]+disp32
#define EA_ECX_DISP32 1		// [ecx]+disp32
#define EA_EDX_DISP32 2		// [edx]+disp32
#define EA_EBX_DISP32 3		// [ebx]+disp32
#define EA_EBP_DISP32 5		// [ebp]+disp32
#define EA_ESI_DISP32 6		// [esi]+disp32
#define EA_EDI_DISP32 7		// [edi]+disp32

// Mod 3
#define EA_REG			// register value (not used)

// Register Types
#define REGT_BYTE 0
#define REGT_WORD 1
#define REGT_DWORD 2
#define REGT_MMX 3
#define REGT_XMM 4
#define REGT_CR 5
#define REGT_DR 6

#pragma pack(push, 1)

typedef struct _OPERAND{
	BYTE op_type;
}OPRND;

typedef struct _INSTRUCTION{
	BYTE code;
	BYTE code_type;
	
	int prefix_1;
	int prefix_2;
	
	OPRND op[3];
	BYTE op_size;
	
	const char *str_code;
	BYTE digit_id;
}INSTRC;

typedef struct _MODRM{
	BYTE Mod;			// 2bit
	BYTE RegOpCode;		// 3bit
	BYTE RM;			// 3bit
}MODRM;

typedef struct _SIB{
	BYTE scale;		// 2bit
	BYTE index;		// 3bit
	BYTE base;		// 3bit
}SIB;

//typedef struct _MODRM_32ADDR{
//	BYTE EffAddrType;
//}

#pragma pack(pop)

class disasm{
public:
	static const INSTRC m_instrc[];
	static const PSTR m_modrm32[][8];
	static const PSTR m_reg[][8];
protected:
	int IsPrefix(BYTE op);			// return index of m_instrc
	int FindOp(BYTE op, BYTE *pos);
	int FindSubOpPrefix(BYTE prefix, BYTE op, BYTE *pos);		// return index of m_instrc
	int FindSubOpPrefix(BYTE prefix, BYTE op1, BYTE op2, BYTE *pos);

	int FindExOp(int iCode, BYTE *opCode);

	void GetModRM(BYTE modRm, MODRM *pModRm);
	int ModRMReqCheck(int iCode);
	int SIBReqCheck(const MODRM &ModRm);
	int DispReqCheck(const MODRM &ModRm);
	int ImmReqCheck(int iCode);

	int GetCodeSize(int iCode);
public:
	disasm();
	virtual ~disasm();

	int GetLineSize(BYTE *code);
};

#endif