#!/bin/bash

#DEBUG="-s -S"
MEM="7802k"

qemu-system-i386 -M microvm $DEBUG -cpu 486 -m "$MEM" -kernel arch/i386/boot/bzImage -nographic -append "console=ttyS0" -serial mon:stdio

