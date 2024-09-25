# 1 llc #
## 1.1 relocation-model(重定位模式) ##
#### 1. `static`
- **描述**: 静态重定位模型。
- **行为**: 在编译时，所有符号地址都被固定，生成的代码不需要在运行时进行重定位。这种模式通常用于生成独立的可执行文件，适合不需要动态库的环境。
- **使用场景**: 固定地址的嵌入式系统、单片程序、无需动态链接的应用。

#### 2. `pic` (Position-Independent Code)
- **描述**: 位置无关代码模型。
- **行为**: 生成的代码可以在内存中的任何位置运行，而不需要重新编译。编译器通过使用相对地址或全局偏移表（GOT）来引用符号地址。这种模型通常用于生成共享库（`.so` 文件）和动态库。
- **使用场景**: 共享库、插件、动态链接库。

#### 3. `pie` (Position-Independent Executable)
- **描述**: 位置无关可执行文件模型。
- **行为**: 类似于 `pic`，但生成的是可执行文件（通常是 `.exe` 或 `.out` 文件）而不是共享库。这种可执行文件在加载时可以在内存中的任何位置运行，增强了安全性，特别是在启用了地址空间布局随机化（ASLR）的系统上。
- **使用场景**: 安全性要求较高的可执行文件，现代操作系统的可执行文件（如 Linux 和 macOS）。

#### 4. `dynamic-no-pic`
- **描述**: 动态非位置无关代码模型。
- **行为**: 生成的代码是动态的，但不是位置无关的。这意味着符号地址在运行时可能需要被重定位，但生成的代码不是位置无关的。这种模式可以用于动态加载的模块，但通常很少使用。
- **使用场景**: 一些特定的动态加载环境，需要更高效的非位置无关代码。

#### 5. `ropi` (Read-Only Position-Independent)
- **描述**: 只读位置无关模型。
- **行为**: 代码段是位置无关的，但数据段需要固定位置。这种模式通常用于嵌入式系统，代码可以在内存中的任何位置运行，但数据必须在固定地址。
- **使用场景**: 嵌入式系统，特别是那些使用只读存储器（ROM）或闪存的系统。

#### 6. `rwpi` (Read-Write Position-Independent)
- **描述**: 读写位置无关模型。
- **行为**: 数据段是位置无关的，但代码段需要固定位置。这种模式允许数据在内存中任何位置进行操作，而代码段固定。
- **使用场景**: 某些嵌入式系统或安全敏感的应用。

#### 7. `ropi-rwpi`
- **描述**: 结合 `ropi` 和 `rwpi` 的模型。
- **行为**: 代码和数据段都是位置无关的。这种模式在嵌入式系统中很常见，允许整个程序在内存中的任何位置运行。
- **使用场景**: 高度灵活的嵌入式系统，允许整个程序在任意位置运行。

#### 总结
不同的重定位模型在不同的场景下提供了灵活性和效率的平衡。通常，`static` 用于静态编译、`pic` 用于共享库、`pie` 用于可执行文件，而其他模型如 `ropi` 和 `rwpi` 则适用于特定的嵌入式环境。

## 1.2 指令描述.td文件中的常见类型与变量
### (1) Operand
Operand<i32>: i32类型操作数,作为一个值存在

### (2) PatLeaf
PatLeaf是模式匹配叶结点，并不会出现在SelDAG中，主要是在pass中发挥作用。这类节点会描述一种模式（图），pass会在SelDAG中寻找这些模式并使用用户提供的方法去处理对应的SelDAGNode。一个例子:

	PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
	imm: 匹配对象
	[...]: 处理方案

### (3) ComplexPattern
ComplexPattern 是 TableGen 中的一种模式，用于描述复杂的匹配逻辑，通常用于内存寻址模式等。与简单的模式（如 PatLeaf）不同，复杂模式允许更灵活的匹配。一个例子:

	ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], [SDNPWantParent]>;
	iPTR: 匹配类型
	2: 操作数
	SelectAddr: 处理方法，自己需要实现
	[frameindex]: 匹配的模式
	[SDNPWantParent]: 额外要求，希望得到父节点信息


### (4) PatFrag
这是一种复杂的匹配模式，可以使用c++自己写条件，给个例子：
	
	class AlignedLoad<PatFrag Node> : 
	  PatFrag<(ops node:$ptr), (Node node:$ptr), [{
	  LoadSDNode *LD = cast<LoadSDNode>(N);
	  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
	}]>; 
	ops: 操作数模式
	Node: 传入的操作
	[]: 里面是自定义的条件

## 1.3 ##
### 调度文件InstrItinClass类 ###
InstrItinClass 是 LLVM 后端中用于描述处理器指令流水线行为的重要类。它为每条指令定义了在处理器不同功能单元中执行的时间表，帮助 LLVM 在指令调度阶段优化指令的执行顺序。通过在 .td 文件中定义 InstrItinClass，开发者可以精确地描述处理器架构的行为，从而生成高效的目标代码。

# 2. 指令选择类

## 2.1 SDLoc
主要包含了SDNode源代码的位置和调试信息，可以定位一个Node对应在IR中的位置。

用法：

    SDLoc DL(N); // N 是某个已经存在的节点，DL 继承了它的位置信息
    SDValue NewNode = DAG.getNode(ISD::SUB, DL, MVT::i32, Operand1, Operand2);
信息会保留在新的节点中,DL也会携带新节点的信息

## 2.2 SDTypeProfile

这是用在.td中的，是节点的类型配置文件，用来约束操作数数量和类型。

	def SDT_Cpu0DivRem : SDTypeProfile<0, 2, [SDTCisInt<0>, SDTCisSameAs<0, 1>] >;

这里0表示输出数量为0，2表示输入数量为2，SDTCinInt<0>限制第0个操作数是int型，SDTCisSameAs<0, 1>表示第 1个操作数与第0个操作数类型相同。

## 2.3 td文件中的模式定义

### (1) 带有set语句的模式
(set Dest, Src) 表示将 Src 赋值给 Dest，其中 Dest 是输出寄存器，Src 是执行某种操作的结果。接下来举个例子：

	(set GPROut:$ra, (OpNode RC:$rb, RC:$rc))
	GPROut:$ra : GRPOut是寄存器类，用来存放输出的，是一种类型修饰，$ra是寄存器变量名，就想函数的形参一样，后面两个也是同理。
	Src = (OpNode RC:$rb, RC:$rc)
	Dest = ra
	表示将OpNode计算的结果放到ra里，定义了这样一种模式


首先一个小括号内部存放一种格式，一个中括号里可以存放一堆小括号。
