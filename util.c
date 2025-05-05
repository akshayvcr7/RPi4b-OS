#include <stdint.h>
#include <stddef.h>

void* memset(void* ptr, int c, size_t size)
{
    char* c_ptr = (char*) ptr;
    for (size_t i = 0; i < size; i++)
    {
        c_ptr[i] = (char) c;
    }
    return ptr;
}

void sys_write(char * buf){
	kprintf(buf);
}

void sys_exit(){
	exit_process();
}

void* const sys_call_table[] = {sys_write, sys_exit};