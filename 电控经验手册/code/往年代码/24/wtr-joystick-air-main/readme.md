# WTR Joystick Air

带有屏幕的自制无线手柄

![遥控器照片](Docs/joystick_photo.jpg)

以下文字是工程模板说明

## EIDE 工程说明

### 目录结构

- CubeMX: 用于存放 CubeMX 的 ioc 文件和 CubeMX 生成的所有文件
- UserCode: 用于存放用户代码文件
- .eide: EIDE 相关的配置文件
  - debug.files.options.yml: 可以给不同的文件指定不同的编译选项

### 编译器选项说明

#### -Wextra

开启额外的 warning，这样会打印更多的 warning，更容易找到 bug

#### 选项：One ELF Section per Function 和 One ELF Section per Data

勾上这两个选项分别等价于添加 `-ffunction-sections` 和 `-fdata-sections` 编译选项。

`-ffunction-sections` 和 `-fdata-sections` 用于告诉编译器将每个函数和数据放在单独的段中，以便在链接时可以更好地优化代码大小。

#### -fno-exceptions

-fno-exceptions 是 GCC 编译器的一个选项，它表示禁用 C++ 异常机制。这个选项会让编译器生成更小、更快的代码，但是会使得程序无法使用 C++ 异常机制。如果你的程序中没有使用 C++ 异常机制，那么可以使用这个选项来优化你的程序。

#### -pipe

-pipe 选项告诉 GCC 使用管道而不是临时文件来在编译的各个阶段之间进行通信。这可以加快编译速度，特别是在 I/O 较慢的系统上。

#### -fno-rtti

-fno-rtti 选项告诉 GCC 不要生成与运行时类型信息有关的代码。这可以减小程序的大小并提高编译速度，但是会使得程序无法使用 RTTI（Run-Time Type Identification）功能。

### 链接器选项说明

#### 选项：Remove Unused Input Sections

勾上这个选项分别等价于添加 `-Wl,--gc-sections` 编译选项。

其中，`-Wl` 是 GCC 的编译选项，用于将后面的参数传递给链接器。

`--gc-sections`用于告诉链接器在链接时删除未使用的段。

因此，配合`-ffunction-sections` 和 `-fdata-sections` 选项可以实现删除未使用的函数和数据（如未使用的全局变量）。

#### --specs=nosys.specs

用于告诉链接器不使用系统调用（system calls）

标准 C 库中的一些功能（比如控制台输入输出、文件读写、获取时间）需要调用操作系统（如 Windows, Linux）提供的函数，这些函数称为系统调用（system calls）。

但是 STM32 没有操作系统，所以也就没有这些函数，于是链接时就会产生找不到这些函数的错误。加上 `--specs=nosys.specs` 之后，链接器就会自己弱定义这些函数，于是就不会产生上述错误了。

链接器自己定义的这些函数不实现任何功能，只是用于去除链接错误的。（所以如果你此时调用`printf`，并不会有任何东西打印出来）

> 当然，你也可以自己实现一些系统调用。例如，实现 `_write()` 和 `_read()` 将 `printf` 重定向到串口。  
> 链接器会优先链接你定义的这些函数。  
> 如果你将所有的系统调用都定义了，也可以不加 `--specs=nosys.specs`，否则就必须加。

#### --specs=nano.specs

用于告诉链接器使用 Nano libc。Nano libc 是一个轻量级的 C 库，它的目标是减小可执行文件的大小。使用 Nano libc 可以减小可执行文件的大小，但是会牺牲一些功能。

默认 Nano libc 关闭了 printf 和 scanf 浮点数的功能。如果要使用该功能，需要在链接器选项中添加 `-u _printf_float` 和 `-u _scanf_float`

> 不添加这一条则会使用标准 C 库。标准 C 库默认支持 printf 和 scanf 浮点数，因此使用标准 C 库时不用添加`-u _printf_float` 和 `-u _scanf_float`

### 链接库

使用 `-lLibraryName` 链接名称为 `LibraryName` 的库。

如：`-lm -lstdc++`

- m：数学库，提供了 math.h 中的各种数学函数。（有时候编译器会默认帮你链接这个库）
- stdc++: C++ 库，提供了许多 C++ 中的函数。

> 编译时使用 `gcc` 命令会默认链接 C 库，不会链接 C++ 库；  
> 编译时使用 `g++` 命令会默认链接 C++ 库；  
> 所以通常情况下使用 `gcc` 编译 .c 文件，使用 `g++` 编译 .cpp 文件  
> 但 EIDE 编译 .cpp 文件时也使用 `gcc` 命令，所以必须指定要链接 C++ 库
