#include <stdio.h>
#include <stdlib.h>

// https://6502.org/users/obelisk/

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

struct Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {
        for (u32 i = 0; i < MAX_MEM; i++) {
            Data[i] = 0;
        }
    }

    /** read 1 byte */
    Byte operator[](u32 Address) const {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    /** write 1 byte */
    Byte & operator[](u32 Address) {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    void WriteWord(Word Value, u32 Address, s32 & Cycles) {
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
        Cycles -= 2;
    }
};

struct CPU {
    Word PC; // program counter
    Word SP; // stack pointer

    Byte A, X, Y; // registers

    Byte C: 1; // status flags
    Byte Z: 1;
    Byte I: 1;
    Byte D: 1;
    Byte B: 1;
    Byte V: 1;
    Byte N: 1;

    void Reset(Mem & memory) {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = Y = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(s32 & Cycles, Mem & memory) {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

	Byte ReadByte(s32 & Cycles, Word Address, Mem & memory) {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Word FetchWord(s32 & Cycles, Mem & memory) {
        // 6502 is little endian
        Word Data = memory[PC];
        PC++;

        Data |= (memory[PC] << 8);
        PC++;

        Cycles -= 2;

        return Data;
    }

	Word ReadWord(s32 & Cycles, Word Address, Mem & memory) {
        Word LoByte = ReadByte(Cycles, Address, memory);
		Word HiByte = ReadByte(Cycles, Address + 1, memory);
        return LoByte |= (HiByte << 8);
    }


    // opcodes
    static constexpr Byte
    INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,
        INS_JSR = 0x20;

    void LDASetStatus() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    s32 Execute(s32 Cycles, Mem & memory) {
        const s32 CyclesRequested = Cycles;
        while (Cycles > 0) {
            Byte Ins = FetchByte(Cycles, memory);
            switch (Ins) {
            case INS_LDA_IM: {
                Byte Value =
                    FetchByte(Cycles, memory);
                A = Value;
                LDASetStatus();
            } break;

            case INS_LDA_ZP: {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();
            } break;

            case INS_LDA_ZPX: {
                Byte ZeroPageAddr =
                    FetchByte(Cycles, memory);
                ZeroPageAddr += X;
                Cycles--;
                A = ReadByte(
                    Cycles, ZeroPageAddr, memory);
                LDASetStatus();
            } break;

			case INS_LDA_ABS: {
				Word AbsAddress = FetchWord(Cycles, memory);
				A = ReadByte(Cycles, AbsAddress, memory);
				LDASetStatus();
			} break;

			case INS_LDA_ABSX:
			{
				Word AbsAddress = FetchWord( Cycles, memory );
				Word AbsAddressX = AbsAddress + X;
				A = ReadByte( Cycles, AbsAddressX, memory );
				if ( AbsAddressX - AbsAddress >= 0xFF )
				{
					Cycles--;
				}
			} break;

			case INS_LDA_ABSY:
			{
				Word AbsAddress = FetchWord( Cycles, memory );
				Word AbsAddressY = AbsAddress + Y;
				A = ReadByte( Cycles, AbsAddressY, memory );
				if ( AbsAddressY - AbsAddress >= 0xFF )
				{
					Cycles--;
				}
			} break;

			case INS_LDA_INDX: {
				Byte ZPAddress = FetchByte( Cycles, memory );
				ZPAddress += X;
				Cycles--;
				Word EffectiveAddr = ReadWord( Cycles, ZPAddress, memory );
				A = ReadByte(Cycles, EffectiveAddr, memory);
				LDASetStatus();
			} break;
            case INS_JSR: {
                Word SubAddr =
                    FetchWord(Cycles, memory);
                memory.WriteWord(
                    PC - 1, SP, Cycles);
                SP += 2;
                PC = SubAddr;
                Cycles--;
            } break;

            default: {
                throw -1;
            } break;
            }
        }
        const s32 CyclesUsed = CyclesRequested - Cycles;
        return CyclesUsed;
    }
};