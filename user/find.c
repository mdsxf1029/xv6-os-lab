#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	
	// 打开路径 
	if ((fd = open(path, 0)) < 0)
	{
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	// 获取路径状态
	if (fstat(fd, &st) < 0)
	{
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return; 
	} 
	// 如果是文件，判断名字是否匹配
	if (st.type != T_DIR)
	{
		if (strcmp(path + strlen(path) - strlen(target), target) == 0)
			printf("%s\n", path);
		close(fd);
		return;
	} 
	// 如果是目录，拼接路径，遍历子文件
	if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
	{
		printf("find: path too long\n");
		close(fd);
		return;
	} 
	
	strcpy(buf, path);      // 把当前路径复制到 buf
	p = buf + strlen(buf);  // p 是 buf 的尾部指针
	*p++ = '/';				// 在后面加一个 '/'，准备拼接子文件名
	
	// 读取一个目录项 de 
	while (read(fd, &de, sizeof(de)) == sizeof(de))
	{
		// 空目录项 
		if (de.inum == 0)
			continue;
		// 当前目录和上一级目录，不进入递归 
		if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
			continue;
			
		memmove(p, de.name, DIRSIZ);   // 文件名拼接到 buf，完整路径 
		p[DIRSIZ] = 0;                 // 加上字符串终止符 
		
		if (stat(buf, &st) < 0)
		{
			printf("find: cannot stat %s\n", buf);
			continue;
		}
		
		if (st.type == T_DIR)
		{
			find(buf, target);
		}
		else
		{
			if (strcmp(de.name, target) == 0)
				printf("%s\n", buf);
		}
	}	
	
	close(fd);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(2, "Usage: find <directory> <filename>\n");
		exit(1);
	}
	
	find(argv[1], argv[2]);
	exit(0);
}
