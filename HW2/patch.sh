#!/bin/sh
dmesg -c
echo '========= HW2 Initializing and Patching ========='
git clone https://github.com/PPPy290351/Linux-Kernel-Programming
cd Linux-Kernel-Programming
patch -p1 < ../0001-homework-done.patch
cd HW2
make
insmod mynull.ko
dmesg
echo '*************************************************************************'
echo 'mknod with the major number, and get the result from "echo" and "cat"   *'
echo 'HW2 DDL is 3/22, keep in mind. :)                                       *'
echo '*************************************************************************'