SRC_FILES := $(filter-out src/main.c src/main_latency.c, $(wildcard src/*.c))
LATENCY_FILES := src/find.c src/hash.c src/init.c src/parsing.c src/utils.c

compile:
	gcc -Wall -Wextra -Werror src/main.c $(SRC_FILES) -o main.out -lm -g

compile_latency:
	gcc -Wall -Wextra -Werror src/main_latency.c $(LATENCY_FILES) -o latency.out -lm -g

r:
	make compile
	./main.out

latency:
	make compile_latency
	./latency.out

v:
	make compile
	valgrind --leak-check=yes --track-origins=yes ./main.out

vm:
	make compile
	valgrind --tool=massif ./main.out

compiledebug:
	gcc src/main.c $(SRC_FILES) -fsanitize=address -g -o main.out -lm

d:
	make compiledebug
	DEBUGINFOD_URLS="" gdb ./main.out

dmac:
	make compiledebug
	lldb ./main.out

compiletest:
	gcc -Wall -Wextra -Werror test/test.c test/utils.c $(SRC_FILES) -o test.out -lm

t:
	make compiletest
	./test.out

compiletestdebug:
	gcc test/test.c test/utils.c -g -o test.out -lm

dt:
	make compiletestdebug
	DEBUGINFOD_URLS="" gdb ./test.out

c:
	rm main.out || true
	rm test.out || true
	rm latency.out || true

f:
	clang-format -i **/*.c **/*.h

cf:
	./check-format.sh
