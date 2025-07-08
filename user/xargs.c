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


























