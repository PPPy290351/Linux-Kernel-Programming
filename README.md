# Linux-Kernel-Programming
Linux kernel course of NTHU <br /> Labs and homework are here

- HW4
    - unistd_*.h :          **arch/x86/include/asm**
    - syscall_table_32.S :  **arch/x86/include/asm**
    - syscalls.h :          **include/linux**

Create a folder at root directory of linux,
and then put the "pyCall.c" into the folder.
also create a new Makefile
```
obj-y := pyCall.o
```

Modify the Makefile which is located at root directory of linux
> find the line which contains "core-y" : kernel/ ....
and then append the folder "myfolder" to this line
```
core-y := kernel/ include/ ....  myfolder/
```

Rebuild
`make`
`make modules_install`
`make install`

reboot and switch to new kernel version

DONE
