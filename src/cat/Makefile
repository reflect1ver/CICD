CFLAG = -Wall -Wextra -Werror -g

all: s21_cat

s21_cat.o: s21_cat.c
	gcc $(CFLAG) -c s21_cat.c

s21_cat: s21_cat.o
	gcc $(CFLAG) s21_cat.o -o s21_cat

clean:
	rm -rf *.o s21_cat

rebuild: clean all
