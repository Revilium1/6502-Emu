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

TEST_F(M6502Test1, LDA_IMCanLoadAValueIntoTheARegister)
{   
    // given
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;

    // when
    CPU cpuCopy = cpu;
    s32 NumCycles = cpu.Execute(2, mem);

    // then
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(NumCycles, 2);

}

TEST_F(M6502Test1, LDA_ZPCanLoadAValueFromTheZPIntoTheARegister)
{
    // given
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x27;
    mem[0x0027] = 0x69;

    // when
    CPU cpuCopy = cpu;
    s32 NumCycles = cpu.Execute(3, mem);

    // then
    EXPECT_EQ(cpu.A, 0x69);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(NumCycles, 3);
}

TEST_F(M6502Test1, LDA_ZPXCanLoadAValueFromTheZPWithOffsetIntoTheARegister)
{
    // given
    cpu.X = 5;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x25;
    mem[0x002A] = 0x73;

    // when
    CPU cpuCopy = cpu;
    s32 NumCycles = cpu.Execute(4, mem);

    // then
    EXPECT_EQ(cpu.A, 0x73);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(NumCycles, 4);
}

TEST_F(M6502Test1, LDA_ZPXCanLoadAValueFromTheZPWithOffsetIntoTheARegisterWhenItWraps)
{
    // given
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x84;

    // when
    CPU cpuCopy = cpu;
    s32 NumCycles = cpu.Execute(4, mem);

    // then
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(NumCycles, 4);
}