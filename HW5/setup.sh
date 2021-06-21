insmod myfs.ko
dd if=/dev/zero of=/tmp/loop0-file bs=1024 count=100
losetup /dev/loop0 /tmp/loop0-file
mkdir /mnt/chi
mount -t myfs /dev/loop0 /mnt/chi/