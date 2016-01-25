#!/bin/sh 

echo "make distclean"
make distclean
echo "make"
make 

rm -rf ~/nfs/rootfs/etc/rcS.d/init.d/occ_apps

sudo cp -a ./occ_apps ~/nfs/rootfs/etc/rcS.d/init.d/
