#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/string.h>

asmlinkage long sys_chishen(char * inputStr)
{
    int len = strlen(inputStr);
    if(len > 100)
        len = 100;
    while(!access_ok(VERIFY_READ, inputStr, len))
        len--;
    char buffer[100];
    int i=0;
    for(;i<len;i++)
        buffer[i] = *(inputStr+i);
    buffer[i] = '\0';
    printk("message: %s\n", buffer);
    return 0;
}