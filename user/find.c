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
	
	// ��·�� 
	if ((fd = open(path, 0)) < 0)
	{
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	// ��ȡ·��״̬
	if (fstat(fd, &st) < 0)
	{
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return; 
	} 
	// ������ļ����ж������Ƿ�ƥ��
	if (st.type != T_DIR)
	{
		if (strcmp(path + strlen(path) - strlen(target), target) == 0)
			printf("%s\n", path);
		close(fd);
		return;
	} 
	// �����Ŀ¼��ƴ��·�����������ļ�
	if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
	{
		printf("find: path too long\n");
		close(fd);
		return;
	} 
	
	strcpy(buf, path);      // �ѵ�ǰ·�����Ƶ� buf
	p = buf + strlen(buf);  // p �� buf ��β��ָ��
	*p++ = '/';				// �ں����һ�� '/'��׼��ƴ�����ļ���
	
	// ��ȡһ��Ŀ¼�� de 
	while (read(fd, &de, sizeof(de)) == sizeof(de))
	{
		// ��Ŀ¼�� 
		if (de.inum == 0)
			continue;
		// ��ǰĿ¼����һ��Ŀ¼��������ݹ� 
		if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
			continue;
			
		memmove(p, de.name, DIRSIZ);   // �ļ���ƴ�ӵ� buf������·�� 
		p[DIRSIZ] = 0;                 // �����ַ�����ֹ�� 
		
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
