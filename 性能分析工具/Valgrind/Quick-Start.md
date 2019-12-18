## 概览
Valgrind工具包提供了许多调试和性能分析工具，可帮助您使程序更快，更正确。这些工具中最流行的是Memcheck。它可以检测C和C++程序中常见的许多与内存相关的错误——这些错误可能导致崩溃和不可预测的行为。

## 准备一段程序
编译代码时加上`-g`，以便Memcheck的错误信息可以包含具体的代码行号。如果可以忍受代码运行慢一点，使用`-O0`也是个不错的主意。虽然通常来说使用`-O1`时Memcheck也能运转良好，并且相对于`-O0`而言速度提升也相当明显，缺点是可能导致错误信息中的行号不准确。不建议使用`-O2`或更高的优化指令，因为Memcheck偶尔会报告实际上不存在的未初始化值错误。

## 在Memcheck下运行你的程序
如果你原来的命令是：
```sh
myprog arg1 arg2
```
那么改为这样
```sh
valgrind --leak-check=yes myprog arg1 arg2
```
以上命令中，Memcheck是默认的工具，`--leak-check`开启详细的内存泄露检测。

你的程序会比平常慢很多（比如20-30倍），并且占用多得多的内存。Memcheck会报告它检测到的有关内存错误和泄漏的消息。

## 解析Memcheck的输出
示例代码`a.c`，包含了一个内存错误和一个内存泄露
```C
  #include <stdlib.h>

  void f(void)
  {
     int* x = malloc(10 * sizeof(int));
     x[10] = 0;        // problem 1: heap block overrun
  }                    // problem 2: memory leak -- x not freed

  int main(void)
  {
     f();
     return 0;
  }
```
大多数错误信息看起来就像下面这样，描述了第一个问题：堆块溢出；
```
  ==19182== Invalid write of size 4
  ==19182==    at 0x804838F: f (example.c:6)
  ==19182==    by 0x80483AB: main (example.c:11)
  ==19182==  Address 0x1BA45050 is 0 bytes after a block of size 40 alloc'd
  ==19182==    at 0x1B8FF5CD: malloc (vg_replace_malloc.c:130)
  ==19182==    by 0x8048385: f (example.c:5)
  ==19182==    by 0x80483AB: main (example.c:11)
```
值得注意的是：
- 每个错误都包含大量信息，要仔细阅读；
- 19282 是进程号，通常不重要；
- 第一行`Invalid write`说明错误的类型，在这里程序在堆块溢出处写了不该写的内存；
- 第一行下面是一个堆栈跟踪，告知问题出在哪里。 堆栈跟踪可能会变得繁杂，尤其是在使用C ++ STL的情况下。 从头开始阅读它们会有所帮助。 如果堆栈跟踪不够大，请使用`--num-callers`选项将其增大。
- 代码地址（例如0x804838F）通常并不重要，但有时对于追踪怪异的bug至关重要。
- 一些错误消息具有第二个层信息描述了所涉及的内存地址。 这表明写入的内存刚好在`example.c`第5行上用`malloc()`分配的块的末尾。

有必要按照报告顺序修复错误，因为后面的错误可能是由先前的错误引起的。未能做到这一点是Memcheck遇到困难的常见原因。

内存泄露的问题长这样子：
```
  ==19182== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
  ==19182==    at 0x1B8FF5CD: malloc (vg_replace_malloc.c:130)
  ==19182==    by 0x8048385: f (a.c:5)
  ==19182==    by 0x80483AB: main (a.c:11)
```

堆栈跟踪告知泄漏内存的分配位置。 不幸的是，Memcheck无法告知为什么产生内存泄漏。 （忽略“ vg_replace_malloc.c”，这是一个实现细节。）

泄露分为好几种，最重要的两种是：
- `definitely lost`: 你的程序正发生内存泄露，赶紧修复！
- `probably lost`:你的程序正在泄漏内存，除非你正使用指针做一些有趣的事情（例如将它们移动到指向堆块中间的位置）。

Memcheck还会报告未初始化值的使用，最常见的消息是“条件跳转或移动取决于未初始化值”。确定这些错误的根本原因可能很困难。 可以尝试使用`--track-origins=yes`获取更多信息。 这会使Memcheck的运行速度变慢，但是获得的额外信息常常可以节省大量时间来确定未初始化变量的位置。

## 注意事项
Memcheck并不完美； 它偶尔会产生误报，我们有一些抑制误报的机制（请参见《 Valgrind用户手册》中的“抑制错误”）。 但是，通常99％的时候是正确的，因此您应该慎重忽略其错误消息。 毕竟，你不会忽略编译器生成的警告消息，对吧？ 如果Memcheck在无法更改的库代码中报告错误信息，则抑制机制也很有用。 默认的抑制设置隐藏了很多这些信息，但你也可能会遇到更多错误信息。