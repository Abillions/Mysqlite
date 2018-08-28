all: main.c logic.c common_func.c
	gcc main.c logic.c common_func.c
clean: a.out
	rm -rf a.out