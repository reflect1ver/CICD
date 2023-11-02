#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Flags {
  int b, e, v, n, s, t, error;
};

//////////////////////////////////////////////////////////////////////////
void parcing(struct Flags *flags, int argc, char *argv[]);
void show_file(struct Flags *flags, char *name);
void with_flags(struct Flags *flags, FILE *fp, int counter_strings);
void print_with_b(int *num, int *i, char line[1024], int *stop_flag,
                  struct Flags *flags, int c);
void print_with_n(int *num, char line[1024], int *i, int *stop_flag,
                  struct Flags *flags, int c);
void print_with_s(int *i, char line[1024], int *stop_flag, struct Flags *flags,
                  int c);
void print(int *i, char line[1024], struct Flags *flags, int c);
void number(int *num);
void flag_init(struct Flags *flags);
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  struct Flags flags;
  flag_init(&flags);
  parcing(&flags, argc, argv);
  return 0;
}

void flag_init(struct Flags *flags) {
  flags->b = 0;
  flags->e = 0;
  flags->v = 0;
  flags->n = 0;
  flags->s = 0;
  flags->t = 0;
  flags->error = 0;
}

void parcing(struct Flags *flags, int argc, char *argv[]) {
  int buf;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "--number-nonblank") == 0) {
        flags->b = 1;
      } else if (strcmp(argv[i], "--number") == 0) {
        flags->n = 1;
      } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
        flags->s = 1;
      } else {
        buf = strlen(argv[i]);
        for (int j = 1; j < buf; j++) {
          if (argv[i][j] == 'b') {
            flags->b = 1;
          } else if (argv[i][j] == 'e') {
            flags->e = 1;
            flags->v = 1;
          } else if (argv[i][j] == 'v') {
            flags->v = 1;
          } else if (argv[i][j] == 'n') {
            flags->n = 1;
          } else if (argv[i][j] == 's') {
            flags->s = 1;
          } else if (argv[i][j] == 't') {
            flags->t = 1;
            flags->v = 1;
          } else if (argv[i][j] == 'T') {
            flags->t = 1;
          } else if (argv[i][j] == 'E') {
            flags->e = 1;
          } else {
            flags->error = 1;
          }
        }
      }
    } else {
      if (flags->error != 1) show_file(flags, argv[i]);
    }
  }
  if (flags->error == 1) printf("s21_cat: illegal option\n");
}

void show_file(struct Flags *flags, char *name) {
  FILE *fp;
  char ARR[1024];
  int counter_strings = 1, j = 0;
  if ((fp = fopen(name, "r")) == NULL) {
    printf("s21_cat: No such file or directory\n");
  } else {
    while ((ARR[j] = fgetc(fp)) != EOF) {
      if (ARR[j] == '\n') {
        counter_strings++;
        j = 0;
      }
      j++;
    }
    fclose(fp);
    fp = fopen(name, "r");
    with_flags(flags, fp, counter_strings);
    fclose(fp);
  }
}

void with_flags(struct Flags *flags, FILE *fp, int counter_strings) {
  int num = 1;
  int i = 0, inner_counter = 0, c = 0;
  int stop_flag = 0;
  char line[1024];
  while (fgets(line, 1024, fp) != NULL) {
    inner_counter++;
    if (inner_counter == counter_strings) c = 1;
    if (flags->b == 1) {
      print_with_b(&num, &i, line, &stop_flag, flags, c);
      continue;
    }
    if (flags->n == 1) {
      print_with_n(&num, line, &i, &stop_flag, flags, c);
      continue;
    }
    if (flags->s == 1) {
      print_with_s(&i, line, &stop_flag, flags, c);
      continue;
    }
    if (flags->e == 1 || flags->t == 1 || flags->v == 1) {
      print(&i, line, flags, c);
      continue;
    } else {
      print(&i, line, flags, c);
    }
  }
}

void print_with_b(int *num, int *i, char line[1024], int *stop_flag,
                  struct Flags *flags, int c) {
  if (line[*i] == '\n') {
    (*i) = 0;
    if (*stop_flag == 0) print(i, line, flags, c);
    if (flags->s == 1) {
      *stop_flag = 1;
    }
  } else {
    (*i) = 0;
    number(num);
    print(i, line, flags, c);
    if (flags->s == 1) {
      *stop_flag = 0;
    }
  }
  (*i) = 0;
}

void print_with_n(int *num, char line[1024], int *i, int *stop_flag,
                  struct Flags *flags, int c) {
  if (line[*i] == '\n') {
    if (*stop_flag == 0) {
      number(num);
      if (flags->e == 1)
        printf("%c%c", '$', '\n');
      else
        printf("%c", '\n');
    }
    if (flags->s == 1) *stop_flag = 1;
  } else {
    number(num);
    print(i, line, flags, c);
    if (flags->s == 1) *stop_flag = 0;
  }
  (*i) = 0;
}

void print_with_s(int *i, char line[1024], int *stop_flag, struct Flags *flags,
                  int c) {
  if (line[*i] == '\n') {
    if (*stop_flag == 0) {
      if (flags->e == 1) {
        printf("$");
      }
      printf("%c", '\n');
    }
    *stop_flag = 1;
  } else {
    (*i) = 0;
    print(i, line, flags, c);
    *stop_flag = 0;
  }
  (*i) = 0;
}

void print(int *i, char line[1024], struct Flags *flags, int c) {
  for (unsigned int j = 0; (c != 1 && line[j] != '\n') || (c == 1 && line[j]);
       j++) {
    if (flags->v == 1 &&
        (line[*i] >= 0 && line[*i] < 32 && line[*i] != 9 && line[*i] != 10)) {
      printf("%c%c", '^', line[*i] + 64);
    } else if (flags->t == 1 && (line[*i] == 9)) {
      printf("%c%c", '^', 'I');
    } else if ((line[*i] == 127) && flags->v == 1) {
      printf("%c%c", '^', '?');
    } else {
      printf("%c", line[*i]);
    }
    (*i)++;
  }
  if (flags->e == 1) {
    if (c != 1) printf("%c%c", '$', '\n');
  } else if (c != 1) {
    printf("%c", '\n');
  }
  (*i) = 0;
}

void number(int *num) {
  int a = *num, len = 0;
  while (a > 0) {
    a /= 10;
    len++;
  }
  while ((6 - len) > 0) {
    printf(" ");
    len++;
  }
  printf("%d\t", (*num)++);
}