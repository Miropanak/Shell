#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/time.h>
#include "func.h"

char * get_time()
{
        //int gettimeofday(struct timeval *tv, struct timezone *tz);
        struct timeval time_v;
        struct timezone time_z;
        int res;
        char * time = malloc(10 * sizeof(char));

        asm(    "push %2\n\t"
                "push %1\n\t"
                "mov $116, %%eax\n\t"
                "push %%eax\n\t"
                "int $0x80\n\t"
                "add $12, %%esp"
                :"=a" (res)
                :"r"(&time_v), "r"(&time_z)
                :"memory");

        int sec = time_v.tv_sec % 86400; //seconds in the day;
        int hour = sec/3600 + 2;
        int minutes = sec%3600/60;
        sprintf(time, "%02d:%02d", hour, minutes);
        return time;
}

char * get_user()
{
        uid_t usr_id;
        struct passwd *pwd;

        asm ( "movl %1, %%eax\n\t"
                          "push %%eax\n\t"
                          "int $0x80"
            : "=a" (usr_id)
            : "r" (SYS_getuid));

        pwd = getpwuid(usr_id);
        return pwd->pw_name;
}

char * get_host()
{
        //sysctl(const int *name, u_int namelen, void *oldp, size_t *oldlenp, const void *newp, size_t newlen);
        int name[2];
        name[0] = CTL_KERN;
        name[1] = KERN_HOSTNAME;
        u_int namelen = 2;
        char * hostname = malloc(50*sizeof(char));
        size_t hostnamelen = 49;
        int service = SYS___sysctl;
        int ret;

        asm ("push %[newlen]\n\t"
                 "push %[newp]\n\t"
                 "push %[oldlenp]\n\t"
                 "push %[oldp]\n\t"
                 "push %[namelen]\n\t"
                 "push %[name]\n\t"
                 "mov %[service], %%eax\n\t"
                 "push %%eax\n\t"
                 "int $0x80\n\t"
                 "add $28, %%esp"
                :"=a" (ret)
                :[service]"r"(service), [name]"r"(name), [namelen]"r"(namelen), [oldp]"r"(hostname),
                 [oldlenp]"r"(&hostnamelen), [newp]"i"(NULL), [newlen]"i"(0)
                :"memory");
        if(ret == 0){
                hostname[hostnamelen] = '\0';
        }
        else{
                perror("sys___sysctl()\n");
                return "-1";
        }
        return hostname;
}

//funkcia na vypisanie promptu
char * print_prompt()
{
        char * prompt = malloc(50 * sizeof(char));
        sprintf(prompt, "%s %s@%s> ", get_time(), get_user(), get_host());
        return prompt;
}
