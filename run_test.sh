#!/bin/bash
echo "Starting cmatest repeated load/unload test"
echo "-----------------------------------------------"
echo "ALLOC_COUNT=0x100000"
ALLOC_COUNT=0x100000

echo "dmesg -C"
dmesg -C

for i in {1..5}; do
    echo "Iteration $i"
    echo "insmod cmatest.ko alloc_count=$ALLOC_COUNT"
    insmod cmatest.ko alloc_count=$ALLOC_COUNT
    echo "sleep 3"
    sleep 3
    echo "rmmod cmatest"
    rmmod cmatest
    echo "sleep 3"
    sleep 3
done

echo "dmesg"
dmesg

echo "-----------------------------------------------"
echo "cmatest repeated load/unload test completed"