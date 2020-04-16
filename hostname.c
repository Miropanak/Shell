#include <stdio.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/syscall.h>

int main()
{
        //sysctl(const int *name, u_int namelen, void *oldp, size_t *oldlenp, const void *newp, size_t newlen);

        int name[2];
        name[0] = CTL_KERN;
        name[1] = KERN_HOSTNAME;
        u_int namelen = 2;
        char hostname[50];
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
                printf("hostname %s\n", hostname);
        }
        else{
                perror("sys___sysctl()\n");
        }
        return 0;
}