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

## Guidance
- 本实验通过多个难度递增的任务帮助我们逐步掌握操作系统核心机制。每个任务的代码量不大，但对底层细节要求极高，强调对内核机制的理解和调试能力的培养。
- 本次实验让我体会到内核级调试和普通程序开发完全不同。我逐渐熟练使用如 GDB、QEMU monitor、print 调试等工具，并意识到每一步调试都需要高度谨慎。通过按步骤实现并配合 Git 快照控制，能有效避免返工。

## Lab Utilities
### 实验概述
本实验我们将熟悉 xv6 及部分重要的系统调用。
#### Boot xv6
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
6. 在 prompt 下输入 ls：
   
    ![](./assets/Lab%20Utilities/ls.png)
    这些是 mkfs 在初始文件系统中包含的文件；大多数是可以运行的程序。输入 ls 就是运行了其中一个。

## Lab System calls

## Lab Page tables

## Lab Traps

## Lab Copy on-write

## Lab Multithreading

## Lab network driver

## Lab Lock

## Lab File System

## Lab mmap