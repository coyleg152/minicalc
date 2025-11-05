.PHONY: all
all:
	gcc minicalc.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o minicalc.out -Wall

.PHONY: debug
debug:
	gcc minicalc.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o minicalc.out -Wall -g

.PHONY: clean
clean:
	rm -f minicalc.out
