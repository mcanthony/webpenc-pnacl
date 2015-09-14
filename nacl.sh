#!/bin/bash

for filename in $(find . -name "Makefile")
do
    sed -i 's/LD = \/usr\/bin\/ld -m elf_x86_64/LD = pnacl-ld/' $filename
    sed -i 's/CC = gcc/CC = pnacl-clang/' $filename
    sed -i 's/PTHREAD_CC = gcc/PTHREAD_CC = pnacl-clang/' $filename
    sed -i 's/ac_ct_CC = gcc/ac_ct_CC = pnacl-clang/' $filename
    sed -i 's/AR = ar/AR = pnacl-ar/' $filename
    sed -i 's/ac_ct_AR = ar/ac_ct_AR = pnacl-ar/' $filename
    sed -i 's/CPP = gcc -E/CPP = pnacl-clang/' $filename
    sed -i 's/-mavx2//' $filename
    sed -i 's/-msse2//' $filename
    sed -i 's/-Wunused-but-set-variable/-Wunused-const-variable/' $filename
    sed -i 's/STRIP = strip/STRIP = pnacl-strip/' $filename
    sed -i 's/RANLIB = ranlib/RANLIB = pnacl-ranlib/' $filename
done
