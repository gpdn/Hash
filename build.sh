#!bin/bash
gcc -Wextra -Wall compiled.c -o compiled
if [[ $? == 0 ]]; then
    read
    exit 0
fi

read
exit 1