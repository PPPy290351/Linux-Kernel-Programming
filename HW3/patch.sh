#!/bin/sh
dmesg -c
echo '================= HW3 Initializing and Patching ================='
git clone https://github.com/PPPy290351/Linux-Kernel-Programming
cd Linux-Kernel-Programming
patch -p1 < ../0001-HW3-done.patch
cd HW3
echo -e '\n>>>>>>> Compiling sigreceiver\n'
gcc -o sigreceiver sigreceiver.c

echo -e '\n>>>>>>> Building module\n'
make
insmod signaltotaskMod.ko
dmesg
echo -e '\n\n'
echo '*******************************************************************************'
echo '1. mknod with the major number, and execute "sigreceiver" in another terminal *'
echo '2. get the pid with "ps aux", then "echo" to your device driver               *'
echo '3. snapshot your process, may output the "signal catched" message             *'
echo 'HW3 DDL is 3/29, keep in mind. :)                               Author: PPPy  *'
echo '*******************************************************************************'