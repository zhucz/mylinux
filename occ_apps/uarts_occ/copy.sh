#!/bin/sh 

rm -rf ~/nfs/rootfs/etc/rcS.d/init.d/uarts_occ 

sudo cp -a ./uarts_occ ~/nfs/rootfs/etc/rcS.d/init.d/
