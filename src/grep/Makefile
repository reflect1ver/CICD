CFLAG = -Wall -Wextra -Werror -g

all: s21_grep

s21_grep.o: s21_grep.c
	gcc $(CFLAG) -c s21_grep.c

s21_grep: s21_grep.o
	gcc $(CFLAG) s21_grep.o -o s21_grep

clean:
	rm -rf *.o s21_grep

rebuild: clean all