# xv6-Based Operating System Lab Report
> 姓名：谢雨霏  
> 学号：2354264  
> 源代码：
> 各实验详细代码可切换至不同 Branch 查看

## Tools
### Installing on Windows
1. 下载 Windows Subsystem for Linux 和 Ubuntu 20.04 from the Microsoft Store。
    在 Windows 中，可以访问"\wsl\$"目录下的所有 WSL 文件。例如，Ubuntu 20.04 的主目录应该在"\wsl\$Ubuntu-20.04\home"
    ```cmd
    C:\Users\mdsxf1029>wsl --install
    适用于 Linux 的 Windows 子系统已安装。
    ```
2. 检查 WSL2 的要求：win+R 打开运行，输入 winver 检查 Windows 版本，版本要求大于 1903
    ![](./assets/winver.png)

3. 安装 Ubuntu
   ```cmd
   C:\Windows\System32>wsl --install -d Ubuntu-20.04
   Ubuntu 20.04 LTS 已安装。
   正在启动 Ubuntu 20.04 LTS...
   Enter new UNIX username: mdsxf_2354264
   New password:
   Retype new password:
   passwd: password updated successfully
   Installation successful!
   ```
5. 下载本课程所需的所有软件
   ```cmd
   $ sudo apt-get update && sudo apt-get upgrade
   $ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
   ```

### Testing Installation
```cmd
$ qemu-system-riscv64 --version
```
![](./assets/test1.png)
```cmd
$ riscv64-linux-gnu-gcc --version
```
![](./assets/test2.png)

---

## Guidance
- 本实验通过多个难度递增的任务帮助我们逐步掌握操作系统核心机制。每个任务的代码量不大，但对底层细节要求极高，强调对内核机制的理解和调试能力的培养。
- 本次实验让我体会到内核级调试和普通程序开发完全不同。我逐渐熟练使用如 GDB、QEMU monitor、print 调试等工具，并意识到每一步调试都需要高度谨慎。通过按步骤实现并配合 Git 快照控制，能有效避免返工。

---

## Lab Utilities
### 实验概述
本实验我们将熟悉 xv6 及部分重要的系统调用。
### Boot xv6
1. 获取实验室的 xv6 源代码并检查 util 分支：
   ```cmd
   $ git clone git://g.csail.mit.edu/xv6-labs-2021
   Cloning into 'xv6-labs-2021'...
   ...
   $ cd xv6-labs-2021
   $ git checkout util
   ```
    ![](./assets/Lab%20Utilities/boot1.png)
2. xv6-labs-2021 仓库与书中的 xv6-riscv 略有不同；它主要增加了一些文件。可使用 `$ git log` 命令查看 git 日志：
   ![](./assets/Lab%20Utilities/log.png)
3. Git 允许我们跟踪对代码所做的更改。例如，如果你完成了某个练习，并想要检查进度，可以运行以下命令提交更改：
   ```cmd
   $ git commit -am 'my solution for util lab exercise 1'
   ```
4. 可以使用命令来跟踪你的更改 `git diff`。运行命令 `git diff` 将显示自上次提交以来的代码更改，并 `git diff origin/util` 显示相对于初始 `xv6-labs-2021` 代码的更改。这里，`origin/xv6-labs-2021` 是你为该课程下载的初始代码所在的 `git` 分支的名称。
5. 构建并运行 xv6：
   ```cmd
   $ make qemu
   ```
   ![](./assets/Lab%20Utilities/qemu1.png)
   ![](./assets/Lab%20Utilities/qemu2.png)
6. 在 prompt 下输入 `ls`：
   
    ![](./assets/Lab%20Utilities/ls.png)

    这些是 mkfs 在初始文件系统中包含的文件；大多数是可以运行的程序。输入 `ls` 就是运行了其中一个。
7. xv6 没有 ps 命令，但是，如果你输入 `Ctrl-p`，内核会打印每个进程的信息。如果你现在尝试一下，你会看到两行：一行是 init，一行是 sh。
   ![](./assets/Lab%20Utilities/ctrlp.png)
8. 要退出 `qemu`，请输入：`Ctrl-a x`。
   ![](./assets/Lab%20Utilities/terminated.png)

---

### sleep
#### 实验目的
1. 为 xv6 实现 UNIX 程序 sleep。
2. 实现的 sleep 应当按用户指定的 ticks 数暂停，其中 tick 是 xv6 内核定义的时间概念，即定时器芯片两次中断之间的时间。解决方案应该放在文件 `user/sleep.c` 中。
#### 实验步骤
1. 阅读 xv6 教材第 1 章，了解 xv6 的用户程序结构和系统调用机制；
2. 查看 `user/echo.c`、`user/grep.c` 等示例程序，学习如何读取命令行参数并进行基本处理；
3. 阅读以下文件，理解 `sleep` 系统调用的实现路径：
   - `user/user.h`：声明了 `sleep(int)` 函数，用户程序可以直接调用；
   - `user/usys.S`：包含从用户态跳转到内核态的汇编代码；
   - `kernel/sysproc.c`：查找 `sys_sleep`，查看内核中 `sleep` 系统调用的实现逻辑。
4. 创建新文件 `user/sleep.c`，并实现如下功能：
   - 判断命令行参数是否存在；
   - 使用 `atoi()` 将参数从字符串转换为整数；
   - 调用系统调用 `sleep(ticks)`；
   - 程序结束前调用 `exit(0)` 正常退出。
5. 代码如下：
```c
#include "kernel/types.h"
#include "user/user.h"

int main() 
{
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    if (fork() == 0)
    {
        // child
        char buf[1];
        read(p2c[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(c2p[1], buf, 1);
        exit(0);
    }
    else
    {
        // parent
        char buf[1] = {'a'};
        write(p2c[1], buf, 1);
        read(c2p[0], buf, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}
```
6. 打开 `Makefile`，找到 `UPROGS` 项；
   ![](./assets/Lab%20Utilities/makefile.png)
7. 添加一行：
   ```bash
   UPROGS = $U/_sleep\
   ```
   ![](./assets/Lab%20Utilities/up.png)
8. 在终端执行以下命令编译 xv6 并启动：
   ```bash
   make qemu
   ```
9.  在 xv6 shell 中输入：
   ```bash
   $ sleep 10
   ```
10. 程序应暂停一段时间后返回，表示 `sleep` 成功运行。
11. 运行测试用例验证实验是否通过：
   ```bash
   make grade
   ```
12. 或者单独测试 `sleep` 题目：
   ```bash
   ./grade-lab-util sleep
   ```
   ![](./assets/Lab%20Utilities/grade.png)
#### 实验中遇到的问题和解决方法
1. vim 编辑器的使用
   - 进入插入模式：按下 `i` 键，开始编辑文本。
   - 退出插入模式回到普通模式：按下 `Esc` 键。
   - 保存并退出：在普通模式下输入 `:wq`，然后按回车（Enter）。
   - 不保存强制退出：在普通模式下输入 `:q!`，然后按回车。
#### 实验心得
- 通过本次 sleep 实验，我对 xv6 操作系统的用户程序设计和系统调用机制有了更深入的理解。实现 sleep 程序让我熟悉了如何从用户态调用内核态的系统调用，以及内核如何通过中断计时器来实现进程的休眠功能。
- 在阅读教材和内核源码时，我逐渐掌握了参数传递、系统调用的汇编跳转流程以及内核中进程睡眠与唤醒的实现方法。编写 user/sleep.c 程序时，遇到了判断命令行参数和调用系统调用的细节问题，通过查阅示例程序和调试，我解决了这些问题并实现了功能。
- 此外，修改 Makefile 添加自定义用户程序，使我体验了完整的编译流程和 xv6 的启动过程，加深了对操作系统构建的理解。最后通过测试用例验证，程序能够按预期暂停指定的 ticks，达到了实验目标。
- 这次实验不仅锻炼了我的 C 语言编程能力，还提高了我阅读和理解操作系统源码的水平，为后续深入学习 xv6 及操作系统相关知识奠定了坚实基础。
   
---

### pingpong
#### 实验目的
1. 掌握 xv6 中进程间通信的基本方法。
2. 练习使用 `pipe()` 实现父子进程间的双向数据传输。
3. 熟悉 `fork()` 创建子进程、`write()`/`read()` 进行数据写入与读取的过程。
4. 理解 `getpid()` 获取当前进程 PID 的机制。
5. 熟悉用户程序的编译流程以及如何将自定义程序添加至 xv6 并执行。
#### 实验步骤
1. 阅读教材第 1 章，理解进程、系统调用和管道（pipe）的基本概念。
2. 编写程序文件
   在 `user/` 目录下创建 `pingpong.c` 文件，实现以下逻辑：
   - 创建两个 pipe，一个用于父进程写子进程读（父->子），一个用于子进程写父进程读（子->父）。
   - 使用 `fork()` 创建子进程。
   - 父进程写入一个字节后，从子进程读取响应。
   - 子进程收到数据后打印 `"pid: received ping"`，再将数据写回父进程，随后退出。
   - 父进程收到子进程数据后打印 `"pid: received pong"`，然后退出。
3. 代码如下：
```c
#include "kernel/types.h"
#include "user/user.h"

int main() 
{
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    if (fork() == 0)
    {
        // child
        char buf[1];
        read(p2c[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(c2p[1], buf, 1);
        exit(0);
    }
    else
    {
        // parent
        char buf[1] = {'a'};
        write(p2c[1], buf, 1);
        read(c2p[0], buf, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}
```
4. 将程序添加到 Makefile
   在 xv6 根目录的 `Makefile` 中找到 `UPROGS` 项，添加：
   ```makefile
   UPROGS = $U/_pingpong\
   ```
5. 编译并运行程序
   ```cmd
   make qemu
   ```
6. 在 xv6 shell 中运行程序
   ```bash
   $ pingpong
   ```
   ![](./assets/Lab%20Utilities/pingpong.png)
7. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20Utilities/grade2.png)
#### 实验中遇到的问题和解决方法
1. pipe 文件描述符数组使用需要小心
   在创建管道后，需要正确理解 `pipe(fd)` 中 `fd[0]` 是读端，`fd[1]` 是写端。如果搞混方向可能导致读写阻塞或出错。在实验中特意多次检查了管道的方向，确保：
   - `p2c[0]` 为父写子读的读端；
   - `c2p[1]` 为子写父读的写端。
2. 调试方式简单但有效
   用 `printf("%d: ...")` 的方式输出 `getpid()` 帮助验证当前是父进程还是子进程，确认流程正确执行，是调试中的有效手段。
#### 实验心得
通过这个实验，我对进程间通信和管道(pipe)的使用有了更深的理解。以前只是理论上知道 pipe 是父子进程共享的一种通信方式，但在实际实现中发现：
- 数据必须以 byte 的方式传递，需要保证先写后读；
- 进程创建后，父子代码同时运行，但逻辑要清晰划分，否则容易出逻辑错误；
- 使用 `fork()` 创建子进程，再结合 `pipe()` 和 `read/write()` 进行通信，体现了 UNIX 下最基本、却也非常强大的系统调用机制。
此外，也体会到在 xv6 这个精简的教学操作系统中完成这些实验，有助于我理解操作系统底层是如何设计系统调用与用户程序之间的接口的，为后续深入学习内核打下了基础。
   
---

### primes
#### 实验目的
1. 理解并掌握使用管道（pipe）和进程（fork）实现并发程序设计的方法；
2. 实现一个基于并发管道的素数筛选算法（Prime Sieve），加深对进程间通信机制的理解；
3. 熟悉 xv6 中的系统调用，包括 `pipe()`、`fork()`、`read()`、`write()`、`wait()` 和 `exit()`；
4. 学习如何通过递归方式构建多个串联的进程，每个进程负责筛去一个质数的倍数。
#### 实验步骤
1. 在 `user/` 目录下创建 `primes.c` 文件；
2. 编写主函数逻辑如下：
   - 创建一个管道 `p[2]`，并使用 `fork()` 创建子进程；
   - 父进程负责将数字 2 到 35 写入管道后退出；
   - 子进程调用 `prime_filter(p[0])` 进行递归处理。
3. 实现递归函数 `prime_filter(int fd)`，完成以下功能：
   - 读取第一个数字 `n`，它就是当前进程要保留的质数；
   - 打印该质数 `n`；
   - 创建新的管道 `next[2]`，并 `fork()`；
     - 父进程继续从当前管道读取数字，筛去所有能被 `n` 整除的数，再将剩下的数字写入 `next[1]`;
     - 子进程递归调用 `prime_filter(next[0])`，处理下一个质数。
   - 父进程关闭所有不再使用的文件描述符，退出。
4. 注意文件描述符管理：
   - 每个进程都要在合适的时机 `close()` 不需要的管道端口；
   - 如果不关闭多余的管道，会导致资源泄漏或死循环。
5. 代码如下：
```c
#include "kernel/types.h"
#include "user/user.h"

void prime_filter(int p_read)
{
	int prime;
	int n;
	
	if (read(p_read, &prime, sizeof(int)) == 0)
	{
		close(p_read);
		exit(0);
	}
	
	printf("prime %d\n", prime);
	
	int next[2];
	pipe(next);
	
	if (fork() == 0)
	{
		close(next[1]);
		close(p_read);
		prime_filter(next[0]);
	}
	else
	{
		close(next[0]);
		while (read(p_read, &n, sizeof(int)))
		{
			if (n % prime != 0)
				write(next[1], &n, sizeof(int));
		}
	
		close(p_read);
		close(next[1]);
		wait(0);
		exit(0);
	}
} 

int main() 
{
    int p[2];
    pipe(p);
    
    if (fork() == 0)
    {
    	close(p[1]);
    	prime_filter(p[0]);
	}
	else
	{
		close(p[0]);
		for (int i = 2; i <= 35; i++)
			write(p[1], &i, sizeof(int));
		close(p[1]);
		wait(0);
		exit(0);
	}
}
```
6. 在 `Makefile` 的 `UPROGS` 项中添加：
   ```makefile
   UPROGS = $U/_primes\
   ```
7. 编译并运行程序
   ```cmd
   make qemu
   ```
8. 在 xv6 shell 中运行程序
   ```bash
   $ primes
   ```
   ![](./assets/Lab%20Utilities/primes.png)
9. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20Utilities/grade3.png)
#### 实验中遇到的问题和解决方法
1. 函数名错误
   - 问题：一开始误用了 `print("...")` 函数来输出内容，导致编译时报错 `implicit declaration of function ‘print’`。
   - 解决方法：查看 `user/user.h` 后发现 xv6 提供的是 `printf()` 而不是 `print()`，将所有输出函数更换为 `printf()` 后成功编译。
2. main 函数缺少退出语句
   - 问题：程序编写完成后运行 `make` 出现错误 `control reaches end of non-void function`。
   - 解决方法：根据报错信息得知 `main()` 是一个 `int` 类型的函数，应该以 `exit(0);` 结束。在 `main()` 函数末尾加上 `exit(0);` 后问题解决。
3. 递归调用结构理解困难
   - 问题：刚开始不理解每个 prime 进程是如何递归创建下一个进程并进行过滤的。
   - 解决方法：反复阅读教材中关于管道 prime sieve 的介绍，并借助画图理解进程间的数据流结构，最终理解递归过滤机制并实现成功。
#### 实验心得
本实验通过实现一个基于管道的并发素数筛选程序，进一步加深了我对 UNIX 系统调用 `fork()`、`pipe()`、`read()`、`write()` 以及进程间通信的理解。与之前的实验相比，本实验引入了递归式进程创建和过滤逻辑，在结构上更为复杂。
在编码过程中，我体会到了资源管理（如及时关闭 pipe）在操作系统中的重要性；否则容易造成文件描述符耗尽等问题。同时，也锻炼了我对递归进程结构的理解和调试能力。
通过本实验，我不仅掌握了 xv6 用户程序中进程与进程之间通信的基本机制，还提高了自己分析问题和解决 bug 的能力，为后续更复杂的实验打下了良好基础。

---

### find
#### 实验目的
1. 实现一个简化版的 UNIX `find` 命令：在目录树中查找具有指定文件名的所有文件。
2. 熟悉文件系统目录的读取与遍历方法。
3. 学习如何使用递归处理目录结构，以及如何正确处理字符串比较和路径拼接。
4. 加强对 `open()`, `read()`, `close()`, `fstat()`, `strcmp()`, `strcpy()` 等系统调用的理解与应用。
5. 掌握 xv6 中如何读取目录项以及如何避免进入当前目录 `.` 和上级目录 `..` 的无限递归。
#### 实验步骤
1. 阅读 xv6 教材，特别是 `user/ls.c` 中对目录读取的实现方法，以了解如何使用 `read()` 和 `struct dirent` 获取目录内容。
2. 编写 find 程序
   在 `user/` 目录下新建文件 `find.c`，实现如下基本功能：
   - 读取命令行参数，提取目标路径和待查找的文件名；
   - 使用 `open()` 打开路径，调用 `fstat()` 判断是否为目录；
   - 如果是文件且名字与给定目标相同，则打印路径；
   - 如果是目录，则递归读取该目录下所有子项，跳过 `"."` 和 `".."`，递归调用 `find()` 继续查找。
3. 代码如下：
```c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(2, "Usage: xargs command [args...]\n");
		exit(1);
	}
	
	// 复制原始命令参数到 args 数组 
	char *args[MAXARG];
	int i;
	for (i = 1; i< argc && i < MAXARG - 2; i++)
		args[i - 1] = argv[i];
		
	char buf[512];
	int n = 0;  // 当前读到的字符数 
	
	while (1)
	{
		char c;
		int r = read(0, &c, 1);  // 从标准输入读取一个字符
		// 发生了错误 
		if (r < 0)
		{
			fprintf(2, "xargs: read error\n");
			exit(1);
		}
		// 读到了文件末尾 
		if (r == 0)
		{
			if (n > 0)
			{
				// 行末未处理，执行命令
				buf[n] = 0;
				args[i - 1] = buf;
				args[i] = 0;
				
				if (fork() == 0)
				{
					exec(args[0], args);
					fprintf(2, "xargs: exec failed\n");
					exit(1);
				} 
				else
					wait(0);
			}
			
			break;
		} 
		// 处理换行符 
		if (c == '\n')
		{
			buf[n] = 0;
			args[i - 1] = buf;
			args[i] = 0;
			
			if (fork() == 0)
			{
				exec(args[0], args);
				fprintf(2, "xargs: exec failed\n");
				exit(1);
			}
			else
				wait(0);
				
			n = 0;  // 重新读取下一行 
		}
		else
		{
			if (n < sizeof(buf) - 1)
			buf[n++] = c;
		} 
	} 
	
	exit(0);
}
```
4. 在 `Makefile` 的 `UPROGS` 项中添加：
   ```makefile
   UPROGS = $U/_find\
   ```
5. 编译并运行程序
   ```cmd
   make qemu
   ```
6. 在 xv6 shell 中运行程序
   ```bash
   $ echo > b
   $ mkdir a
   $ echo > a/b
   $ find .b
   ```
   ![](./assets/Lab%20Utilities/find.png)
7. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20Utilities/grade4.png)
#### 实验中遇到的问题和解决方法
1. 字符串处理理解困难
   - 问题描述：对 `strcmp(path + strlen(path) - strlen(target), target) `这一判断语句不理解，误以为是路径比较操作。
   - 解决方法：通过调试和辅助讲解，理解这是在判断路径末尾的文件名是否与目标文件名相同，用于处理普通文件的情况。
#### 实验心得
通过本次实验，我深入理解了 UNIX 中文件系统的目录结构和递归遍历机制，也熟悉了如何使用系统调用如 `open`、`read`、`fstat`、`stat` 来进行底层文件与目录的操作。实验中最大的收获是掌握了如何处理目录项结构 `dirent`，以及通过字符串拼接构造递归路径的方法。
此外，我也巩固了对 C 语言字符串处理函数如 `strcmp`、`strcpy`、`strlen` 和 `memmove` 的使用。这个实验让我体会到了编写系统层程序所需的细心与逻辑性，对我理解操作系统中“文件”这一抽象概念非常有帮助。
在今后的学习中，我会继续加强对指针、结构体以及系统调用使用的掌握，为后续更复杂的 xv6 实验打好基础。

---

### xargs
#### 实验目的
1. 实现一个简化版的 UNIX `xargs` 程序，该程序读取标准输入中的每一行，并对每一行调用一次指定的命令，将该行作为参数追加到命令末尾执行。
2. 熟悉 xv6 中用户程序的编写流程，包括：
   - 使用 `fork()` 创建子进程；
   - 使用 `exec()` 执行指定命令；
   - 使用 `wait()` 等待子进程完成；
3. 掌握如何从标准输入中读取数据并进行逐行处理。
4. 学会将多个用户程序（如 `find`、`grep`、`xargs`）组合使用，实现更强大的功能。
#### 实验步骤
1. 理解 xargs 的工作原理：
   为了更好地实现 xv6 上的 `xargs`，我们首先通过以下示例理解其行为：
   ```bash
   $ echo hello too | xrags echo bye
   bye hello too
   ```
   说明：`xargs` 将标准输入的内容 `"hello too"` 作为参数追加给命令 `"echo bye"`，组合成命令 `echo bye hello too`，最终输出 `bye hello too`。
   注意：在 UNIX 中，`xargs` 默认会尽可能多地将输入拼接为命令参数。为了避免这一优化行为对本实验的影响，我们使用 `-n 1` 参数来实现逐行传参：
   ```bash
   $ echo -e "1\n2" | xargs -n 1 echo line
   line 1
   line 2
   ```
   每一行单独构成一个命令执行，行为更符合我们在 xv6 实验中的实现目标。
2. 编写程序 `user/xargs.c`，功能如下：
   - 从标准输入逐行读取内容；
   - 将读取的每一行作为参数传给指定命令；
   - 利用 `fork()` 创建子进程，使用 `exec()` 执行目标命令；
   - 父进程使用 `wait()` 等待子进程结束后再继续处理下一行；
   - 所有参数存储在 `argv[MAXARG]` 中，记得包含原始命令行参数。
3. 程序实现要点：
   - 使用 `read()` 每次读取一个字符，遇到换行符 `'\n'` 将整行处理；
   - 使用 `fork()` + `exec()` 创建进程并执行命令；
   - `exec()` 参数需要以 `char* argv[]` 的形式组织，末尾加 NULL 结束；
   - 注意添加基本错误处理（如输入过长、fork 失败等）。
4. 代码如下：
```c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(2, "Usage: xargs command [args...]\n");
		exit(1);
	}
	
	// 复制原始命令参数到 args 数组 
	char *args[MAXARG];
	int i;
	for (i = 1; i< argc && i < MAXARG - 2; i++)
		args[i - 1] = argv[i];
		
	char buf[512];
	int n = 0;  // 当前读到的字符数 
	
	while (1)
	{
		char c;
		int r = read(0, &c, 1);  // 从标准输入读取一个字符
		// 发生了错误 
		if (r < 0)
		{
			fprintf(2, "xargs: read error\n");
			exit(1);
		}
		// 读到了文件末尾 
		if (r == 0)
		{
			if (n > 0)
			{
				// 行末未处理，执行命令
				buf[n] = 0;
				args[i - 1] = buf;
				args[i] = 0;
				
				if (fork() == 0)
				{
					exec(args[0], args);
					fprintf(2, "xargs: exec failed\n");
					exit(1);
				} 
				else
					wait(0);
			}
			
			break;
		} 
		// 处理换行符 
		if (c == '\n')
		{
			buf[n] = 0;
			args[i - 1] = buf;
			args[i] = 0;
			
			if (fork() == 0)
			{
				exec(args[0], args);
				fprintf(2, "xargs: exec failed\n");
				exit(1);
			}
			else
				wait(0);
				
			n = 0;  // 重新读取下一行 
		}
		else
		{
			if (n < sizeof(buf) - 1)
			buf[n++] = c;
		} 
	} 
	
	exit(0);
}
```
5. 在 `Makefile` 的 `UPROGS` 项中添加：
   ```makefile
   UPROGS = $U/_xargs\
   ```
6. 编译并运行程序
   ```cmd
   make qemu
   ```
7. 在 xv6 shell 中运行程序
   ```bash
   make clean
   make qemu
   ```
   ![](./assets/Lab%20Utilities/xargs.png)
8. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20Utilities/grade5.png)

#### 实验中遇到的问题和解决方法
1. xv6 shell 多次打印 `$` 提示符
   - 问题描述：运行 `sh < xargstest.sh` 时提示符 `$` 多次打印，看起来混乱。
   - 原因解释：这是 xv6 shell 在处理脚本文件输入时的正常行为，虽然不美观，但不影响实验结果。
#### 实验心得
本实验让我深入理解了 `xargs` 的工作机制，特别是在 UNIX 管道命令组合中的重要作用。通过自己实现一个简化版本的 `xargs`，掌握了以下几个关键点：
1. 标准输入的字符级读取处理技巧：理解了如何通过 `read()` 函数逐字符读取输入，并根据换行符来分隔参数；
2. `exec` 和 `fork` 的配合使用：熟悉了父进程通过 `fork()` 创建子进程，子进程通过 `exec()` 执行具体命令的过程；
3. 如何构造 `argv[]` 数组：学会了如何将字符串转化为命令参数数组，并在 `exec()` 中传入；
4. 调试技巧的提升：通过查看报错信息、打印调试信息，以及结合 `Makefile` 管理用户程序，提升了排查问题的能力。

总的来说，xargs 实验不仅锻炼了我的系统调用使用能力，也加深了我对 UNIX 命令组合思想和进程管理的理解，为之后更复杂的 shell 或系统工具开发打下了良好基础。

---

### Util 分支测试样例通过结果
![](./assets/Lab%20Utilities/gradeall.png)

---
## Lab System calls

---
## Lab Page tables

## Lab Traps

## Lab Copy on-write

## Lab Multithreading

## Lab network driver

## Lab Lock

## Lab File System

## Lab mmap