# 体系架构 #
## 寄存器 ##
32 位 RISC 架构；

### (1) 16个通用寄存器，R0 到 R15； ###

- R0 是常数 0 寄存器（CR, Constant Register）
- R1-R10 是通用寄存器（GPR, General Purpose Register）
- R11 是全局指针寄存器（GP, Global Pointer register）
- R12 是帧指针寄存器（FP, Frame Pointer register）
- R13 是栈指针寄存器（SP, Stack Pointer register）
- R14 是链接寄存器（LR, Link Register）
- R15 是状态字寄存器（SW, Status Word register)

		通用寄存器的名字
		R1 R2 R3 R4 R5 R6 R7 R8 R9 R10
		AT V0 V1 A0 A1 T9 T0 T1 S0 S1

### (2) 协处理寄存器，PC 和 EPC； ###

- PC 是程序计数器（Program Counter）
- EPC 是错误计数器（Error Program Counter）

### (3)其他寄存器 ###

- IR 是指令寄存器（Instruction Register）
- MAR 是内存地址寄存器（Memory Address Register）
- MDR 是内存数据寄存器（Memory Data Register）
- HI 是 MULT 指令的结果的高位存储（HIgh part）
- Lo 是 MULT 指令的结果的低位存储（LOw part）

### (4)寄存器编号 ###
## 栈 ##
- 8字节对齐

栈帧模型，文本从上往下对应地址从高到低

	//  0                 ----------
	//  4                 Args to pass
	//  .                 saved $GP  (used in PIC)
	//  .                 Alloca allocations
	//  .                 Local Area
	//  .                 CPU "Callee Saved" Registers
	//  .                 saved FP
	//  .                 saved RA
	//  .                 FPU "Callee Saved" Registers
	//  StackSize         -----------
FPU是什么东西？浮点计算单元？可是实际上cpu0参考中并没有提到这一点
理论上来说之后需要在某个地方指明参与这个过程的寄存器和顺序

RA寄存器对应的是LR,存的是调用前的PC值

## 指令集 ##

![3类指令](cpu_imgs/1-指令格式.png)

### 汇编格式 ###
记录几条特殊的指令

	//  -- Frame directive "frame Stackpointer, Stacksize, RARegister"
	//  Describe the stack frame.
	//
	//  -- Mask directives "(f)mask  bitmask, offset"
	//  Tells the assembler which registers are saved and where.
	//  bitmask - contain a little endian bitset indicating which registers are
	//            saved on function prologue (e.g. with a 0x80000000 mask, the
	//            assembler knows the register 31 (RA) is saved at prologue.
	//  offset  - the position before stack pointer subtraction indicating where
	//            the first saved register on prologue is located. (e.g. with a
	//
	//  Consider the following function prologue:
	//
	//    .frame  $fp,48,$ra
	//    .mask   0xc0000000,-8
	//       addiu $sp, $sp, -48
	//       st $ra, 40($sp)
	//       st $fp, 36($sp)

这里基本都是注释，不过很有用。
## 5级流水线 ##
取指（IF, Instruction Fetch）、解码（ID, Instruction Decode）、执行（EX, EXecute）、内存访问（MEM, MEMory access）、写回（WB, Write Back）。

内存访问是Load/Store时有用，写回是Load时有用。

这一块与调度有关，为什么呢？

## 子目标 ##
两种子目标：Cpu0I和Cpu0II，这两者的指令有些不同。

### Cpu0I ###
使用Cmp比较，比较结果会保存在SW的位里
### Cpu0II ###
使用Slt比较，比较结果直接保存在某个通用寄存器里
# 2 ABI #




## 2.2 数据类型的大小和对齐方式 ##

定义基本数据类型（如 int、float、double）的大小，以及它们在内存中的对齐方式。

### 2.2.1 大端和小端 ###
- 大端：高位字节在低地址
- 小端：高位字节在高地址

以0x12345678为例：

    地址 00 01 02 03
	大端 12 34 56 78
	小端 78 56 34 12
大端和小端的名字来源：来自于《格列佛游记》，两个国家争论鸡蛋该从大的那一边（大端）还是小的那一边（小端）打破。引申到CS大端就是从数字比较大（不讨论符号，高位数字更大）的那一端开始存，也就是高位字节在低地址。而小端就是从数字比较小（低位数字更小）的字节开始存，也就是高位字节在高地址。
## 系统调用接口 ##

定义应用程序如何调用操作系统提供的功能，比如文件操作、内存管理等。

## 二进制文件格式 ##

定义可执行文件、动态库、静态库的格式。例如，Windows 上使用的 PE 格式，Linux 上使用的 ELF 格式。

## 寄存器使用约定 ##

定义哪些寄存器由调用者保存，哪些由被调用者保存。这确保了函数调用前后的寄存器状态是可预测的。
- 返回值存放寄存器: V0, V1, A0, A1

## 堆栈帧布局 ##

定义函数栈帧的结构，包括局部变量的存储位置、返回地址的位置、保存的寄存器等。
