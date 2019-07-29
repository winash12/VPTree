clang++ -Wall -std=c++17 -O3 -march=native -Ofast -ftree-vectorize -I/usr/local/include -c -g   *.cpp

clang++ -o coo *.o -l Geographic
