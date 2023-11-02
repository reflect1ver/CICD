#!/bin/bash

output1=$(clang-format -n src/cat/*.c 2>&1)
output2=$(clang-format -n src/grep/*.c 2>&1)

if [[ -n $output1 ]] || [[ -n $output2 ]]; then
    clang-format -n src/cat/*.c
    clang-format -n src/grep/*.c
    exit 1
fi 