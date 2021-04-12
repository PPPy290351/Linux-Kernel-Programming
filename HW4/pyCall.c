#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/string.h>

asmlinkage long sys_chishen(char * inputStr)
{
    int len = strlen(inputStr);
    if(len > 100)
        len = 100;
    printk("======================= pointer verifier ======================================\n");
    while(!access_ok(VERIFY_READ, inputStr, len)){
        printk("Access denied\n");
        return -1;
    }
    printk("=============================================================\n");
    char buffer[100];
    int i=0;
    for(;i<len;i++)
        buffer[i] = *(inputStr+i);
    if(i == 100){
        buffer[i] = '\0';
    }
    printk("message: %s\n", buffer);
    return 0;
}