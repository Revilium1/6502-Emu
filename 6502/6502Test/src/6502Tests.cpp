#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1 : public testing::Test
{
public:
    Mem mem;
    CPU cpu;
    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        cpu.Reset(mem);
    }
    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }
};

static void VerifyUnmodifiedRegFromLDA(const CPU& cpu, const CPU& cpuCopy)
{
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(M6502Test1, TheCPUDoesNothingWhenNoCyclesAreRequested)
{   
    //given
    constexpr s32 NUM_CYCLES = 0;

    //when
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    //then
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_IMCanLoadAValueIntoTheARegister)
{   
    // given
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    CPU cpuCopy = cpu;
    constexpr s32 NUM_CYCLES = 2;

    // when
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);

}

TEST_F(M6502Test1, TheCPUCanExecuteMoreCyclesThanRequestedIfTheInstructionRequiresIt)
{   
    // given
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    constexpr s32 NUM_CYCLES = 1;

    // when
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(CyclesUsed, 2);

}

TEST_F(M6502Test1, LDA_IMCanAffectTheZeroFlag)
{   
    // given
    cpu.A = 0x42;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x00;
    CPU cpuCopy = cpu;
    constexpr s32 NUM_CYCLES = 2;

    // when
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);

}

TEST_F(M6502Test1, LDA_ZPCanLoadAValueFromTheZPIntoTheARegister)
{
    // given
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x27;
    mem[0x0027] = 0x69;
    constexpr s32 NUM_CYCLES = 3;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x69);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_ZPXCanLoadAValueFromTheZPWithOffsetIntoTheARegister)
{
    // given
    cpu.X = 5;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x25;
    mem[0x002A] = 0x73;
    constexpr s32 NUM_CYCLES = 4;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x73);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_ZPXCanLoadAValueFromTheZPWithOffsetIntoTheARegisterWhenItWraps)
{
    // given
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x84;
    constexpr s32 NUM_CYCLES = 4;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_ABSCanLoadAValueFromAnAddressIntoTheARegister)
{
    // given
    mem[0xFFFC] = CPU::INS_LDA_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0x69;
    constexpr s32 NUM_CYCLES = 4;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x69);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, 4);
}
TEST_F(M6502Test1, LDA_ABSXCanLoadAValueFromAnAddressIntoTheARegister)
{
    // given
    cpu.X = 0x01;
    mem[0xFFFC] = CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 NUM_CYCLES = 4;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_ABSYCanLoadAValueFromAnAddressIntoTheARegister)
{
    // given
    cpu.Y = 0x01;
    mem[0xFFFC] = CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x2C;
    constexpr s32 NUM_CYCLES = 4;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x2C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_ABSXCanLoadAValueFromAnAddressIntoTheARegisterWhenItCrossesABoundary)
{
    // given
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4402
    mem[0x4501] = 0x3F;
    constexpr s32 NUM_CYCLES = 5;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x3F);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}
TEST_F(M6502Test1, LDA_ABSYCanLoadAValueFromAnAddressIntoTheARegisterWhenItCrossesABoundary)
{
    // given
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4402
    mem[0x4501] = 0x21;
    constexpr s32 NUM_CYCLES = 5;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x21);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_INDXCanLoadAValueIntoTheARegister)
{
    // given
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00;
    mem[0x0007] = 0x80;
    mem[0x8000] = 0x3D;
    constexpr s32 NUM_CYCLES = 6;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0x3D);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_INDYCanLoadAValueIntoTheARegister)
{
    // given
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0xFA;
    constexpr s32 NUM_CYCLES = 5;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0xFA);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDA_INDYCanLoadAValueIntoTheARegisterWhenPageCrossed)
{
    // given
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8101] = 0xFA;
    constexpr s32 NUM_CYCLES = 6;

    // when
    CPU cpuCopy = cpu;
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then
    EXPECT_EQ(cpu.A, 0xFA);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedRegFromLDA(cpu, cpuCopy);
    EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}