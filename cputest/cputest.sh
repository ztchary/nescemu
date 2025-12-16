gcc -Wall -Werror src/* -Iinclude ../src/cpu.c -o cputest
./cputest > nestest.log2
diff nestest.log nestest.log2 | head -5
