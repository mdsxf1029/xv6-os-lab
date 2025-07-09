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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }
  int ticks = atoi(argv[1]);
  sleep(ticks);
  exit(0);
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

   exit(0)
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
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 递归查找函数
void find(char *path, char *target) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // 打开路径
  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // 获取路径状态
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // 如果是文件，判断名字是否匹配
  if (st.type != T_DIR) {
    if (strcmp(path + strlen(path) - strlen(target), target) == 0) {
      printf("%s\n", path);
    }
    close(fd);
    return;
  }

  // 是目录：拼接路径，遍历子文件
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    printf("find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0) {
      printf("find: cannot stat %s\n", buf);
      continue;
    }

    if (st.type == T_DIR) {
      find(buf, target);  // 递归进入子目录
    } else {
      if (strcmp(de.name, target) == 0) {
        printf("%s\n", buf);  // 找到目标文件
      }
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <directory> <filename>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
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
### 实验概述
本实验是对 xv6 操作系统系统调用机制的深入实践。通过实现新的系统调用，学习用户态程序如何与内核交互，了解系统调用在内核中的注册、调度与执行流程。实验的主要目标是实现两个系统调用：`trace` 和 `sysinfo`，分别用于追踪系统调用的使用情况与获取系统的运行状态信息。
在开始编码之前，请阅读 xv6 book 的第 2 章、第 4 章的第 4.3 节和第 4.4 节以及相关源文件：
- 系统调用的用户空间代码在 `user/user.h` 和 `user/usys.pl` 中。
- 内核空间代码是 `kernel/syscall.h` 和 `kernel/syscall.c`。
- 与进程相关的代码是 `kernel/proc.h` 和 `kernel/proc.c`。
- 开始实验，切换到 `syscall` 分支：
   ```cmd
   $ git fetch
   $ git checkout syscall
   $ make clean
   ```
### System call tracing
#### 实验目的
本实验的目标是为 xv6 操作系统添加一个系统调用追踪机制。通过实现一个名为 `trace(int mask)` 的系统调用，使用户程序能够根据掩码（mask）选择性地追踪某些系统调用。该追踪机制将在每次系统调用返回时，在终端输出该系统调用的名称、调用进程的 PID 以及返回值。
该功能对调试系统调用、理解系统调用执行过程非常有帮助，特别是在后续实验中对系统调用行为的观察与验证。通过本实验，学生将掌握以下关键技能：
- 如何向 xv6 添加一个新的系统调用；
- 用户态到内核态的参数传递过程；
- 内核态如何输出调试信息；
- 如何在子进程中继承父进程的 trace 状态；
- 理解 xv6 的系统调用处理流程。
#### 实验步骤
1. 理解追踪功能设计要求
   - `trace(mask)` 接收一个整数参数，表示需要追踪的系统调用集合，使用掩码的方式实现；
   - 若某次系统调用编号对应的位在掩码中被设置（即 `mask & (1 << syscall_number) != 0`），则在系统调用返回前输出：
      ```php-template
      <pid>: syscall <name> -> <return_value>
      ```
2. 在 `Makefile` 的 `UPROGS` 项中添加：
   ```makefile
   UPROGS = $U/_trace\
   ```
3. 添加系统调用接口
   - 打开 `kernel/syscall.h`，添加 `SYS_trace` 的系统调用编号；
      ```c
      #define SYS_trace 22
      ```
   - 在 `user/user.h` 中声明用户态函数原型:
      ```c
      int trace(int);
      ```
   - 修改 `user/usys.pl` 文件，添加：
      ```scss
      entry("trace");
      ```
4. 实现 trace 系统调用逻辑
   - 在 `kernel/proc.h` 的 `struct proc` 中添加字段 `int trace_mask`，用于记录每个进程的追踪掩码；
   - 在 `kernel/sysproc.c` 中添加新函数：
      ```c
      uint64
      sys_trace(void)
      {
         int mask;
         if (argint(0, &mask) < 0) 
            return -1;
         myproc()->trace_mask = mask;
         return 0;
      }
      ```
5. 让子进程继承追踪状态
   - 修改 `kernel/proc.c` 中的 `fork()` 函数，使得子进程继承父进程的 `trace_mask`：
      ```c
      np->trace_mask = p->trace_mask;
      ```
6. 在 syscall 返回前输出追踪信息
   - 修改 `kernel/syscall.c` 中的 `syscall()` 函数：
     - 添加一个包含所有系统调用名称的字符串数组 `syscall_names`；
     - 在 syscall 返回前，根据 `myproc()->trace_mask` 判断是否输出信息；
         ```c
         if (p->trace_mask & (1 << num))
            printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num], p->trapframe->a0);
         ```
7. 在 `kernel/syscall.c` 中添加 trace 调用：
   - 添加 sys_trace() 的外部声明
      ```c
      extern uint64 sys_trace(void);
      ```
   - 添加 syscalls 函数指针的对应关系
      ```c
      static uint64 (*syscalls[])(void) = {
         ...
         [SYS_trace] sys_trace,  
      };
      ```
8. 验证追踪功能是否正确
   - `trace` 调用 grep 只跟踪 read 系统调用。
      ![](./assets/Lab%20System%20calls/test1.png)
   - `trace` 运行 grep，同时跟踪所有系统调用；2147483647 的 31 位低位全部被设置。
      ![](./assets/Lab%20System%20calls/test2.png)
   - 程序没有被跟踪，因此没有打印跟踪输出。
      ![](./assets/Lab%20System%20calls/test3.png)
   - `$ trace 2 usertests forkforkfork`，跟踪了 usertests 中 `forkforkfork` 测试的所有后代的 fork 系统调用。
      ![](./assets/Lab%20System%20calls/test41.png)
      ![](./assets/Lab%20System%20calls/test42.png)
9. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20System%20calls/grade1.png)

#### 实验中遇到的问题和解决方法
1. 访问系统调用返回值出错
   - 问题：不知道如何获取系统调用的返回值。
   - 解决方法：查阅资料后发现可以从当前进程的 `trapframe->a0` 中获取系统调用的返回值：
      ```c
      printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num], p->trapframe->a0);
      ```
#### 实验心得
本次实验深入理解了系统调用在 xv6 操作系统中的实现过程，特别是用户空间调用系统调用如何通过 `ecal`l 指令切换到内核空间，并在内核中找到对应的处理函数进行处理。通过添加 `trace()` 系统调用，并在 `syscall()` 中打印调用信息，锻炼了以下几个方面的能力：
- 理解了 `trapframe` 的作用，知道了如何读取返回值或参数；
- 学会了添加一个新的系统调用所需的完整流程，包括声明、编号、实现、用户态调用；
- 熟悉了 xv6 进程结构 `proc`，并能对其进行扩展以保存追踪信息；
- 提高了对调试和日志追踪机制的认识，了解了 trace 对后续实验调试的帮助。
  
整个实验思路清晰，实现难度适中，但对系统调用机制和内核处理流程有了更深刻的认识，为后续实现更复杂的系统功能打下了良好基础。

---

### Sysinfo
#### 实验目的
本实验旨在通过实现一个新的系统调用 `sysinfo`，帮助理解系统调用的参数传递与内核态到用户态的数据交互过程，并进一步熟悉 xv6 中内存管理和进程管理的相关机制。通过本实验，学生将：
- 掌握添加一个带结构体参数并向用户态返回数据的系统调用的完整流程；
- 熟悉 xv6 中如何统计空闲内存和活跃进程数量；
- 理解如何使用 `copyout()` 将内核态结构体数据复制到用户空间；
- 了解 `kalloc.c` 和 `proc.c` 中与资源统计相关的数据结构与逻辑。
#### 实验步骤
1. 添加用户态声明
   - 在 `user/user.h` 中添加声明（注意要前向声明 struct）：
      ```c
      struct sysinfo;
      int sysinfo(struct sysinfo *);
      ```
2. 添加用户态系统调用包装
   - 在 user/usys.pl 中添加一行：
      ```perl
      entry("sysinfo");
      ```
3. 分配系统调用号
   - 在 `kernel/syscall.h` 中添加新的 syscall 编号，例如：
      ```c
      #define SYS_sysinfo 23
      ```
4. 添加内核态处理函数
   - 在 `kernel/sysproc.c` 中添加函数 `sys_sysinfo()`：
      - 调用 `argaddr()` 获取用户态指针参数；
      - 在内核中调用 `copyout()` 将内核中的 `struct sysinfo` 数据复制回用户空间。
         ```c
         uint64
         sys_sysinfo(void)
         {
            struct sysinfo info;
            uint64 addr;  // 用户空间的地址

            // 获取参数：sysinfo 结构体的地址（用户空间）
            if (argaddr(0, &addr) < 0)
               return -1;

            // 填充结构体内容
            info.freemem = freemem();   // 自己在 kalloc.c 中实现
            info.nproc = count_proc();  // 自己在 proc.c 中实现

            // 复制 info 到用户空间
            if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
               return -1;

            return 0;
         }
         ```
5. 内核逻辑实现
   - 在 `kernel/proc.c` 中添加函数 `count_proc()`：
     - 遍历 `proc[]` 表格，统计状态不是 `UNUSED` 的进程数量；
         ```c
         int
         count_proc(void)
         {
            struct proc *p;
            int count = 0;

            for (p = proc; p < &proc[NPROC]; p++)
            {
               if (p->state != UNUSED)
                  count++;
            }

            return count;
         }
         ```
   - 在 `kernel/kalloc.c` 中添加函数 `freemem()`：
     - 遍历 `kmem` 空闲链表，计算可分配内存总量。
         ```c
         uint64
         freemem(void)
         {
            struct run *r;
            uint64 free = 0;

            acquire(&kmem.lock);
            r = kmem.freelist;
            while (r)
            {
               free += PGSIZE;
               r = r->next;
            }
            release(&kmem.lock);

            return free;
         }
         ```
   - 并在 `kernel/defs.h` 中声明。
6. 添加结构体定义
   - 在 `kernel/sysinfo.h` 中定义 `struct sysinfo`：
      ```c
      struct sysinfo {
         uint64 freemem;
         uint64 nproc;
      };
      ```
7. 注册系统调用
   - 在 `kernel/syscall.c`：
     - 添加 `extern uint64 sys_sysinfo(void);`
     - 在 `syscalls[]` 数组中注册 `sys_sysinfo`
8. 测试验证
   - 在 `Makefile` 的 `UPROGS` 项中添加：
      ```makefile
      UPROGS = $U/_sysinfotest\
      ```
   - 运行测试
     - 我们提供了一个测试程序 `sysinfotest`；如果打印出 `"sysinfotest： OK"`，则实验通过。
         ```sh
         $ make qemu
         $ sysinfotest
         ```
         ![](./assets/Lab%20System%20calls/sys.png)
#### 实验中遇到的问题和解决方法
1. `copyout()` 使用报错或无效
   - 问题原因：初次调用 `copyout()` 不熟悉其用法，未正确传入 `pagetable` 和地址。
   - 解决方法：参考 `sys_fstat()` 的写法，传入 `myproc()->pagetable`, 用户地址 `info_addr`，以及结构体地址 `(char*)&info` 和大小。
#### 实验心得
通过本实验对 xv6 操作系统添加 `sysinfo` 系统调用的过程，我更加深入地理解了系统调用从用户态到内核态的完整流程，包括：
- 系统调用号的注册 (`syscall.h`)
- 用户态函数声明和调用（`user.h` 与 `usys.pl`）
- 内核函数的实现 (`sysproc.c`)
- 内核向用户态传递结构体数据的方式（`copyout()` 的使用）
  
此外，本实验还训练了我对 xv6 源码中模块间关系的理解，尤其是进程管理（`proc.c`）和内存管理（`kalloc.c`）的接口调用方式。通过统计空闲内存和活动进程数的实现，我学会了如何从内核层收集系统运行时信息，并安全地传递给用户态程序。
这个实验虽然不算特别复杂，但细节较多，要求对 xv6 架构有清晰的理解，对后续系统调用相关实验奠定了良好基础。

---

### Syscall 分支测试样例通过结果
![](./assets/Lab%20System%20calls/gradeall.png)

---

## Lab Page tables
### 实验概述
本实验中将探索页面表并对其进行修改，以加快某些系统调用并检测哪些页面已被访问。
在开始编码之前，请阅读 xv6 一书的第 3 章及相关文件：
- `kern/mlayout.h`，用于捕捉内存布局。
- `kern/vm.c`，包含大部分虚拟内存（VM）代码。
- `kernel/kalloc.c`，包含分配和释放物理内存的代码。
- 开始实验，切换到 `pgtbl` 分支：
   ```cmd
   $ git fetch
   $ git checkout pgtbl
   $ make clean
   ```
### Speed up system calls
#### 实验目的
一些操作系统（如 Linux）通过在用户空间和内核之间共享只读区域中的数据来加快某些系统调用的速度。这样，在执行这些系统调用时就不需要内核交叉了。本实验旨在学习如何在页表中插入映射，首先需要在 xv6 中的 `getpid()` 系统调用中实现这一优化。
通过在用户空间和内核之间共享一个只读区域中的数据，来加速特定的系统调用。具体而言，通过在进程创建时映射一个只读页，将一个 `struct usyscall` 结构放置在该页的开头，该结构会存储当前进程的 `PID`。这将使得在执行某些系统调用时，不需要进行用户空间和内核之间的频繁切换，从而提高系统调用的性能。
#### 实验步骤
1. 在 `kernel/proc.h` 的 proc 结构体中添加指针来保存这个共享页面的地址。
   ```c
   struct usyscall *usyscall;
   ```
2. 在 `kernel/proc.c` 的 `allocproc()` 函数中，为每个新创建的进程分配一个只读页，使用 `mappages()` 来创建页表映射：
   在进程表中查找未使用的进程，如果找到，初始化在内核中运行所需的状态，并在 p->lock 保持不变的情况下返回；如果没有空闲的进程，或者内存分配失败，则返回 0。
```c
static struct proc*
allocproc(void)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Allocate a usyscall page.
  if((p->usyscall = (struct usyscall *)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }
  p->usyscall->pid =  p->pid ; 

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  return p;
}
```
3. `kalloc` 函数分配的是物理内存，我们还需要完成从虚拟地址到物理地址的映射，这一过程需要在 `kernel/proc.c` 的 `proc_pagetable()` 函数中，利用 `mappages()` 函数实现。
   `proc_pagetable() `函数用于为进程创建一个用户页表。用户页表用于映射用户进程的虚拟地址到物理地址，但在此过程中，还会映射一些特殊页，如 `trampoline` 和 `trapframe` ，我们需要为其增加一个到 `usyscall` 的映射：
```c
pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  if(pagetable == 0)
    return 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  // int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)
  if(mappages(pagetable, TRAMPOLINE, PGSIZE,
              (uint64)trampoline, PTE_R | PTE_X) < 0){
    uvmfree(pagetable, 0);
    return 0;
  }

  // map the trapframe just below TRAMPOLINE, for trampoline.S.
  if(mappages(pagetable, TRAPFRAME, PGSIZE,
              (uint64)(p->trapframe), PTE_R | PTE_W) < 0){
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }
    
  // map the usyscall page.
  if(mappages(pagetable, USYSCALL, PGSIZE,
              (uint64)(p->usyscall), PTE_U | PTE_R) < 0){
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}
```
4. 我们还需要在必要的时候释放页面，比如终止进程时，我们需要在 `kernel/proc.c` 的 `freeproc()` 函数中，依葫芦画瓢，将我们分配的 `usyscall` 和 `trapframe` 页面做相同处理，添加相关代码：
```c
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  p->trapframe = 0;
  if(p->usyscall)
    kfree((void*)p->usyscall);
  p->usyscall = 0;
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
}
```
5. 在 xv6 shell 中运行程序
   ```bash
   make clean
   make qemu
   $ pgtbltest
   ```
   ![](./assets/Lab%20Page%20tables/test.png)
6. 运行测试用例验证实验是否通过：
   ![](./assets/Lab%20Page%20tables/grade1.png)

#### 实验中遇到的问题和解决方法
1. 页面映射问题：
   - 问题描述：在进行页面映射时，我遇到了页表相关的映射问题，导致系统调用执行失败或崩溃，比如我在运行的时候得到了下面这样的报错：
      ```bash
      xv6 kernel is booting

      hart 2 starting
      hart 1 starting
      panic: freewalk: leaf
      ```
   - 解决方法：发现 `panic` 是由 `kernel/vm.c` 的 `freewalk()` 函数发出的，分析函数调用结构，我们可以发现，还需要在 `kernel/proc.c` 的 `proc_freepagetable()` 函数中释放我们之前建立的虚拟地址到物理地址的映射，将这段代码修改为：
      ```c
      void
      proc_freepagetable(pagetable_t pagetable, uint64 sz)
      {
         uvmunmap(pagetable, TRAMPOLINE, 1, 0);
         uvmunmap(pagetable, TRAPFRAME, 1, 0);
         uvmunmap(pagetable, USYSCALL, 1, 0);
         uvmfree(pagetable, sz);
      }
      ```
#### 实验心得
通过本次实验，首先我看到了性能优化的重要性，这给了我很大的启示：通过将系统调用的相关数据放在只读页中，以减少内核和用户空间之间的数据传输次数，从而加速系统调用的执行。
此外这个实验使我更深入地理解了系统调用的工作原理以及它们是如何在用户空间和内核空间之间进行通信的。这为我提供了更清晰的操作系统工作流程的认识。通过在每个进程的页表中插入只读页，掌握操作页表的方法，从而实现用户空间与内核空间之间的数据共享。尽管在实验过程中遇到了不少问题，但是这让我对映射与回收、权限授予等有了更深刻的理解。

---

### Print a page table
#### 实验目的
深入理解 `RISC-V` 页表的结构和内容，并提供一个打印页表的函数 `vmprint()`。通过这个实验，实现可视化页表的布局，了解页表的层次结构以及如何将虚拟地址映射到物理地址。
#### 实验步骤
1. 按照题目要求，在 `kernel/exec.c` 中插入 `if(p->pid==1) vmprint(p->pagetable)`：
2. 在 `kernel/vm.c` 中添加 `vmprint()` 函数的代码，由于 `vmprint()` 仅接收一个 `pagetable_t` 类型的参数，而我们在递归过程中还需要一个参数来标记递归深度，因此我们设置一个辅助函数 `vmprint_recursive()` ，参数类型为 `pagetable_t` 和 `int`：
```c
 void
 vmprint_recursive(pagetable_t pagetable, int level)
 {
   for (int i = 0; i < 512; i++)
   {
     pte_t pte = pagetable[i];
     if (pte & PTE_V) {
       uint64 pa = PTE2PA(pte);
       printf("..");
       for (int j = 0; j < level; j++) {
         printf(" ..");
       }
       printf("%d: pte %p pa %p\n", i, pte, pa);
       // PTE without any WRX bit set points to low-level page table
       if ((pte & (PTE_W|PTE_R|PTE_X)) == 0)
         vmprint_recursive((pagetable_t)pa, level + 1);
     }
   }
 }
 ​
 void 
 vmprint(pagetable_t pagetable)
 {
   printf("page table %p\n", pagetable);
   vmprint_recursive(pagetable, 0);
 }
```
3. 在 `kernel/defs.h` 文件中增加一个函数定义
   ```c
   void vmprint(pagetable_t pagetable);
   ```
4. 启动 xv6，可以看到打印出来的页表
   ```cmd
   make qemu
   ```
   ![](./assets/Lab%20Page%20tables/vmprint.png)

#### 实验中遇到的问题和解决方法
1. 递归错误：在递归地遍历页表时，起初在递归的设计上，思路还不太清晰，后来参考了 `freewalk` 函数的形式，让我对递归遍历的逻辑有了更好的理解。
2. 格式化输出问题：我在格式化输出的语法上出现了一些错误。后来我结合控制台调试，使用 `%p` 格式化符正确地打印了 64 位的十六进制 PTE 和物理地址。
#### 实验心得
通过本次实验，我们可以清晰地通过 `vmprint()` 的输出来查看页表的层次结构，从根页表开始，逐级向下指向不同级别的页表页，最终到达最底层的页表页，其中包含了实际的物理页框映射信息。在用户模式下，进程可以读取第 1 页映射的内容，但不能写入。倒数第三页通常包含用户栈的映射信息。此外我们还可以观察到，输出的物理地址可能与示例不同，这属于正常的实验现象，这帮助我们更好地理解页表是如何将虚拟地址映射到物理地址的。
此外，这个实验加深了我对页表结构的理解，并且学会了如何在内核中操作位操作和宏定义，以及如何通过递归遍历页表来打印出整个页表的内容。

---

### Detecting which pages have been accessed
#### 实验目的
一些垃圾回收器（一种自动内存管理形式）可以从哪些页面已被访问（读取或写入）的信息中获益。在这部分实验中，您将为 xv6 添加一项 `pgaccess()` 系统调用，通过检查 RISC-V 页表中的访问位来检测并向用户空间报告这些信息。每当 RISC-V 硬件走页器解决 TLB 未命中问题时，都会在 PTE 中标记这些位。
`pgaccess()` 用于报告哪些页面已被访问。系统调用需要三个参数。首先，它需要第一个要检查的用户页面的起始虚拟地址。其次，它接受要检查的页面数。最后，它需要一个缓冲区的用户地址，以便将结果存储到位掩码（一种数据结构，每页使用一位，其中第一页对应的是最小有效位）中。
本实验的目的是向 xv6 内核添加一个新特性，即通过检查 RISC-V 页表中的访问位，实现一个系统调用 `pgaccess()`，该系统调用可以报告哪些页面已被访问（读取或写入）。这对于垃圾回收器等应用有一定的益处。
#### 实验步骤
1. 依据提示，根据 xv6 手册内容，我们应在 `kernel/riscv.h` 内定义 `PTE_A` ：
   ```c
   #define PTE_A (1L << 6)
   ```
2. 定义 `sys_pgaccess()`
   - 在 `kernel/sysproc.c` 中定义 `sys_pgaccess()` ，查阅相关代码发现 `sys_pgaccess()` 相关的入口、函数声明等都已给出，仅定义 `sys_pgaccess()` 即可；
   - 根据题目要求和提示， `sys_pgaccess()` 系统调用有三个参数，包括两个地址和一个整型，对于系统调用，应该用 `argint()` 和 `argaddr()` 来读取参数；
   - 开始计算掩码，起始 `mask` 应初始化为 0 ，然后开始遍历从 `va` 开始的页面，如果当前页面对应的页表项（利用 `walk()` 函数得到）的 `PTE_A = 1` ，则将掩码对应位置 1 ，置 1 操作可通过或运算完成；
   - 根据提示，在检查 PTE_A 是否被设置之后，需要将其清除（置 0 ）。否则，将无法确定自上次调用 `pgaccess()` 以来页面是否被访问过，换句话说，该位将被永久置位。为将 PTE_A 置 0 ，我们将其取反与页表项相与即可，最终的代码如下所示：
      ```c
      int
      sys_pgaccess(void)
      {
         uint64 va;
         int page_nums;
         uint64 user_addr;
  
         if(argaddr(0, &va) < 0)
            return -1; 
         if(argint(1, &page_nums) < 0)
            return -1;
         if(argaddr(2, &user_addr) < 0)
            return -1;
    
         uint64 mask = 0;
         uint64 complement = ~PTE_A;  // 访问位清零 
  
         struct proc *p = myproc();
         for(int i = 0; i < page_nums && i < 64; ++i)
         {
            pte_t *pte = walk(p->pagetable, va + i * PGSIZE, 0);
            if (pte && (*pte & PTE_A))
            {
    	         mask |= (1L << i);
    	         *pte &= complement;
	         }
         }
  
         if(copyout(p->pagetable, user_addr, (char *)&mask, sizeof(mask)) < 0)
            return -1;
  
         return 0;
      }
      ```
3. `walk()` 函数未在 `kernel/defs.h` 中声明，需要在添加这一声明。
   ```c
   pte_t* walk(pagetable_t pagetable, uint64 va, int alloc);
   ```
4. 编译并运行程序
   ```cmd
   make qemu
   ```
5. 在 xv6 shell 中运行程序
   ```bash
   $ pgtbltest
   ```
   ![](./assets/Lab%20Page%20tables/access.png)
#### 实验中遇到的问题和解决方法
1. 访问位未清除导致测试失败
   - 问题描述：在实现中只读取了访问位（PTE_A），但未清除，导致多次调用 `pgaccess()` 时无法反映新的访问状态，造成测试用例失败。
   - 解决方法：在判断 `PTE_A` 被设置后，使用按位与清除访问位：
      ```c
      *pte &= ~PTE_A;
      ```
#### 实验心得
通过这个实验，我学习了操作系统的内存管理机制，包括页表的结构和作用；理解了如何为进程分配页表，映射虚拟地址到物理地址，以及如何使用页表权限来实现不同的访问控制。
在实现系统调用过程中，我深入了解了内核代码的组织结构和运行方式，以及如何将用户态的请求转换为内核态的操作，并了解了如何从用户空间传递参数到内核空间。
此外我还能够结合教材中的页表结构来编写程序，这更让我理解了上层算法实现和底层硬件逻辑之间的紧密联系，这为我今后的学习打下基础、积累了一定的经验。

---
### Pgtbl 分支测试样例通过结果
![](./assets/Lab%20Page%20tables/grade2.png)

---

## Lab Traps
### 实验概述
这个实验将会探索系统调用是如何使用陷阱（trap）实现的。首先将会利用栈做一个热身练习，接下来你将会实现一个用户级陷阱处理（user-level trap handling）的例子。
- `kernel/trampoline.S`：从用户空间到内核空间并返回的汇编代码。
- `kernel/trap.c`：处理所有中断的代码。
- 开始实验，切换到 `traps` 分支：
   ```cmd
   $ git fetch
   $ git checkout traps
   $ make clean
   ```
### RISC-V assembly
#### 实验目的
1. 理解系统调用过程：通过阅读 `call.c` 和 `call.asm`，掌握用户态程序如何通过 `ecall` 发起系统调用；
2. 熟悉 RISC-V 汇编语言：识别函数参数寄存器、返回地址、函数调用指令等；
3. 理解调用栈与寄存器的作用：掌握 `jalr`, `ra`, `a0-a7` 等寄存器的具体作用；
4. 掌握大小端的影响：通过字节序实验理解 little-endian 与 big-endian 的差异；
5. 完成 traps 部分测试并编写 answers-traps.txt：对照源码与汇编，解答给定问题，完成报告提交。
#### 实验步骤
1. 编译 xv6 并生成汇编文件
   - 编译用户程序和文件系统镜像，这一步会自动生成 `user/call.asm`，这是 `user/call.c` 编译后的汇编版本。
      ```bash
      make fs.img
      ```
2. 回答实验问题（填写 `answers-traps.txt`）
在 xv6 根目录下新建 `answers-traps.txt` 文件，并回答以下问题：
   - Q1：Which registers contain arguments to functions? For example, which register holds 13 in main's call to `printf`?
     - `a1`, `a2`, `a3` 等通用寄存器将保存函数的参数。
         ![](./assets/Lab%20Traps/q1.png)
     - 查看 `call.asm` 文件中的 `main` 函数可知，在 `main` 调用 `printf` 时，由寄存器 `a2` 保存 13。
   - Q2：Where is the call to function `f` in the assembly code for main? Where is the call to `g`? (Hint: the compiler may inline functions.)
     - 查看 `call.asm` 文件中的 `f` 和 `g` 函数可知，函数 `f` 调用函数 `g`；函数 · 使传入的参数加 3 后返回。
         ![](./assets/Lab%20Traps/f.png)
     - 此外，编译器会进行内联优化，即一些编译时可以计算的数据会在编译时得出结果，而不是进行函数调用。查看 `main` 函数可以发现，`printf` 中包含了一个对 f 的调用。但是对应的会汇编代码却是直接将 f(8)+1 替换为 12 。
         ![](./assets/Lab%20Traps/inline.png)
   - Q3：At what address is the function `printf` located?
     - 查阅得到其地址在 `0x630`。
         ![](./assets/Lab%20Traps/q3.png)
   - Q4：What value is in the register `ra` just after the `jalr` to `printf` in `main`?
     - 30：使用 `auipc ra,0x0` 将当前程序计数器 `pc` 的值存入 `ra` 中。
     - 34：`jalr 1536(ra)` 跳转到偏移地址 `printf` 处，也就是 `0x630` 的位置。
     - 在执行完这句命令之后，寄存器 `ra` 的值设置为 `pc + 4`，也就是 `return address` 返回地址 `0x38`。即 `jalr` 指令执行完毕之后，`ra` 的值为 `0x38`。
   - Q5：Run the following code. What is the output? The output depends on that fact that the RISC-V is little-endian. If the RISC-V were instead big-endian what would you set `i` to in order to yield the same output? Would you need to change `57616` to a different value?
      ```c
      unsigned int i = 0x00646c72;
      printf("H%x Wo%s", 57616, &i);
      ```
     - 运行结果：打印出了 `He110 World`。
     - 首先，`57616` 转换为 16 进制为 `e110`，所以格式化描述符 `%x` 打印出了它的 16 进制值。
     - 其次，如果在小端（little-endian）处理器中，数据 `0x00646c72` 的高字节存储在内存的高位，那么从内存低位，也就是低字节开始读取，对应的 ASCII 字符为 `rld`。
     - 如果在大端（big-endian）处理器中，数据 `0x00646c72` 的高字节存储在内存的低位，那么从内存低位，也就是高字节开始读取其 ASCII 码为 `dlr`。
     - 所以如果需要满足大端序和小端序输出相同的内容 `i`，那么在其为大端序的时候，`i` 的值应该为 `0x726c64`，这样才能保证从内存低位读取时的输出为 `rld`。
     - 无论 `57616` 在大端序还是小端序，它的二进制值都为 `e110` 。大端序和小端序只是改变了多字节数据在内存中的存放方式，并不改变其真正的值的大小，所以 `57616` 始终打印为二进制 `e110`。
     - 因此，如果在大端序，`i` 的值应该为 `0x00726c64` 才能保证与小端序输出的内容相同。且不需要改变 `57616` 的值。
   - Q6：In the following code, what is going to be printed after `'y='`? (note: the answer is not a specific value.) Why does this happen?
      ```c
      printf("x=%d y=%d", 3);
      ```
     - 因为函数的参数是通过寄存器 `a1`, `a2` 等来传递。如果 `printf` 少传递一个参数，那么其仍会从一个确定的寄存器中读取其想要的参数值，但是我们没有给出并存储这个确定的参数，所以函数将从此寄存器中获取到一个随机的不确定的值作为其参数。故而此例中，`y=` 后面的值我们不能够确定，它是一个垃圾值。
#### 实验中遇到的问题和解决方法
1. 寄存器混淆
   - 问题描述：刚接触 RISC-V 汇编时不熟悉 `a0~a7`、`ra`、`sp` 等寄存器的功能。
   - 解决方法：查阅 RISC-V calling convention 规范文档，弄清楚哪些寄存器用于传参、返回值、保存返回地址等。例如：`a0` 是第一个参数也是返回值，`ra` 是返回地址。
2. 格式化字符串参数不匹配引发混淆
   - 问题描述：运行 `printf("x=%d y=%d", 3);` 发现输出中 `y=` 后是随机值。
   - 解决方法：明白格式字符串和参数数量不匹配时，会从栈上读取未定义的内容，出现“垃圾值”，属于未定义行为。
#### 实验心得
通过本次实验，我对RISC-V 汇编语言和系统调用底层机制有了更深入的理解。尤其是在阅读 `call.asm` 的过程中，我掌握了：
   - RISC-V 的函数调用规范（a0-a7 传参、ra 保存返回地址）；
   - 汇编语言是如何展开 C 函数调用的，尤其是内联函数处理；
   - 系统调用的实现过程以及 `ecall` 指令的作用；
   - `printf()` 的格式化字符串如何对应参数传递；
   - 大小端存储方式对内存访问的影响，尤其是字符串拼接时的影响。

此外，通过手动分析 `call.asm` 的过程，也提升了我阅读和理解底层代码的能力。这个实验让我意识到，理解系统调用与硬件架构之间的联系，是掌握操作系统原理的关键一步。

---

### Backtrace
#### 实验目的
实现一个回溯（`backtrace`）功能，用于在操作系统内核发生错误时，输出调用堆栈上的函数调用列表。这有助于调试和定位错误发生的位置。
#### 实验步骤
1. 

#### 实验中遇到的问题和解决方法

---

### Alarm
#### 实验目的

#### 实验步骤
1. 编译并运行程序
   ```cmd
   make qemu
   ```
2. 在 xv6 shell 中运行程序
   ```bash
   $ pgtbltest
   ```
   ![](./assets/Lab%20Page%20tables/access.png)
#### 实验中遇到的问题和解决方法

#### 实验心得

---
### Traps 分支测试样例通过结果
![]()

---

## Lab Copy on-write

## Lab Multithreading

## Lab network driver

## Lab Lock

## Lab File System

## Lab mmap