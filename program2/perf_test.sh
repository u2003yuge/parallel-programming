
g++ $1.cpp -g -o $1  -fopenmp -lpthread -std=c++11
# perf record -e L1-dcache-load-misses,L1-dcache-loads,L1-icache-load-misses,L1-icache-loads ./$1
# perf record -e branch-misses,cache-misses ./$1
perf record -e L1-dcache-load-misses,L1-dcache-loads,branch-misses,cache-misses,cycles,instructions ./$1
# perf stat ./$1