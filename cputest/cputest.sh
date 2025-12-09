gcc cputest.c ../src/cpu.c -I../include -o cputest
./cputest > nestest.log2
diff nestest.log nestest.log2 | head -5
