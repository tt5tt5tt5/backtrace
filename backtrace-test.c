#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <dlfcn.h>

typedef struct
{
    const char *dli_fname;  /* File name of defining object.  */
    void *dli_fbase;        /* Load address of that object.  */
    const char *dli_sname;  /* Name of nearest symbol.比如函数名*/
    void *dli_saddr;        /* Exact value of nearest symbol.比如函数的起始地址*/
} Dl_info;


struct ucontext_ce123 {
    unsigned long     uc_flags;
    struct ucontext  *uc_link;
    stack_t       uc_stack;
    struct sigcontext uc_mcontext;
    sigset_t      uc_sigmask;   /* mask last for extensibility */
}ucontext_ce123_;


struct sigframe_ce123 {
    struct sigcontext sc;//保存一组寄存器上下文
    unsigned long extramask[1];
    unsigned long retcode;//保存返回地址
    //struct aux_sigframe aux __attribute__((aligned(8)));
}sigframe_ce123;



int backtrace (void **BUFFER, int SIZE) 
{ 
int n = 0; 
int *p = &n; 
int i = 0; 

int ebp = p[1]; 
int eip = p[2]; 

for(i = 0; i < SIZE; i++) 
{ 
	printf("eip = 0x%x,ebp=0x%x \n",*(int*)eip,*(int*)ebp);
	BUFFER[i] = (void*)eip; 
	p = (int*)ebp; 
	ebp = p[0]; 
	eip = p[1]; 
} 

return SIZE; 
} 


void dumpstack()
{


    Dl_info info;
    int status;


    int *fp = 0, *next_fp = 0;
    int cnt = 0;
    int ret = 0;


    __asm__(
        "mov %0, fp\n"
        : "=r"(fp)
    );

	printf("fp 0x%x\n",*fp);

	
    next_fp = (int *)((fp-3));
	printf("next_fp 3 0x%x\n",*next_fp);
	next_fp = (int *)((fp-2));
	printf("next_fp 2 0x%x\n",*next_fp);
	next_fp = (int *)(*(fp-1));
	printf("next_fp 1 0x%x\n",*next_fp);
	//next_fp = (int *)((fp-0));
	//printf("next_fp 0 0x%x\n",*next_fp);
	
    while(next_fp != 0)
    {
        void * pcur = (void *)(next_fp);


        status = dladdr (pcur, &info);
        if (status && info.dli_fname && info.dli_fname[0] != '\0')
        {
            printf("0x%08x 0x%08x %-20s <  %s+%p  >\r\n",pcur,info.dli_saddr,info.dli_fname,info.dli_sname,(unsigned long)((unsigned int)pcur - (unsigned int)info.dli_saddr));
        }
        else
         {
            printf ( "[%p] \n", *(int *)(next_fp));
			printf("ezio status = %d \n",status);
         }


        //array[cnt++] = (void *)*(next_fp - 1);
        next_fp = (int *)(*(next_fp-1));
        cnt++;
    }
	sleep(1);

    printf("Backstrace (%d deep)\n", cnt);
}



static void my_sighdlr (int sig)
{
	if (sig == SIGSEGV)
        { // Ignore SIGPIPE.
        	sleep(1);
			void * bt[20];
			backtrace(bt,20);
   			dumpstack();
			printf("sig = %d\n",sig);
			//exit();
            return;
        }
	sleep(1);
	//go = 0;
}

int testfun(int n)
{
	printf("n = %d\n",n);
	dumpstack();
	if(n>=10)
	{
		//memset(0,0,100);
		//void * bt[20];
		//backtrace(bt,20);
		sleep(1);
		return 0;
	}
	n = n+1;
	sleep(1);
	testfun(n);
}
int eziotestfun()
{
	printf("eziotestfun");
	sleep(1);
	dumpstack();
	testfun(0);
}
int ezioTEST()
{
	printf("ezioTEST");
	sleep(1);
	dumpstack();
	eziotestfun();
}
int main()
{
	signal(SIGSEGV, my_sighdlr);
	ezioTEST();

return 0;
}
