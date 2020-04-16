#include <stdio.h>
#include <sys/time.h>

int main()
{
	//int gettimeofday(struct timeval *tv, struct timezone *tz);
	struct timeval time_v;
	int res;

	asm("push %1\n\t"
		"mov $116, %%eax\n\t"
		"push %%eax\n\t"
		"int $0x80\n\t"
		"add $12, %%esp"
		:"=a" (res)
		:"r"(&time_v)
		:"memory");

	printf("%ld sec\n", time_v.tv_sec);
	int sec = time_v.tv_sec % 86400; //seconds in the day;
	int hour = sec/3600 + 2;
	int minutes = sec%3600/60;
	printf("%02d:%02d", hour, minutes);
	return 0;
}