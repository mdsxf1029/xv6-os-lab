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
  
  exit(0);
}
