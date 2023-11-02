#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Flags {
  int e, i, v, c, l, n, error;
};

/////////////////////////////////////////////////////////////////////
void parcing(struct Flags *flags, int argc, char *argv[],
             char file_names[256][256], char words[256][256], int *w_cu,
             int *f_cu);
void show_file(struct Flags *flags, char file_names[256][256],
               char words[256][256], int *w_cu, int *f_cu);
void flag_init(struct Flags *flags);
int compare(char argv[], char massive[256][256], int cu);
void to_lower(char str[]);
void show(char line[2048]);
int no_e(char *argv[], int argc);
void count_str(int w_cu, char line[2048], char words[256][256], int *c);
void with_flags(struct Flags *flags, FILE *fp, char words[256][256], int w_cu,
                char *f, int f_cu, int *c, int *l, char Lfiles[256][256]);
void print_with_i(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f);
void print_with_v(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f);
void print_with_c(int f_cu, char *f, int c);
int print_with_l(int w_cu, char line[2048], char words[256][256]);
void print_with_n(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f, int num);
void print_without_flags(int w_cu, char line[2048], char words[256][256],
                         int f_cu, char *f);
/////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  struct Flags flags = {0};
  int w_cu = 0, f_cu = 0;
  char file_names[256][256] = {0};
  char words[256][256] = {0};
  flag_init(&flags);
  parcing(&flags, argc, argv, file_names, words, &w_cu, &f_cu);
  if (flags.error == 1) {
    printf("s21_grep: illegal option\n");
  } else {
    show_file(&flags, file_names, words, &w_cu, &f_cu);
  }
  return 0;
}

void flag_init(struct Flags *flags) {
  flags->e = 0;
  flags->i = 0;
  flags->v = 0;
  flags->c = 0;
  flags->l = 0;
  flags->n = 0;
  flags->error = 0;
}

void parcing(struct Flags *flags, int argc, char *argv[],
             char file_names[256][256], char words[256][256], int *w_cu,
             int *f_cu) {
  int buf;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      buf = strlen(argv[i]);
      for (int j = 1; j < buf; j++) {
        if (argv[i][j] == 'e') {
          flags->e = 1;
          continue;
        }
        if (argv[i][j] == 'i') {
          flags->i = 1;
          continue;
        }
        if (argv[i][j] == 'v') {
          flags->v = 1;
          continue;
        }
        if (argv[i][j] == 'c') {
          flags->c = 1;
          continue;
        }
        if (argv[i][j] == 'l') {
          flags->l = 1;
          continue;
        }
        if (argv[i][j] == 'n') {
          flags->n = 1;
          continue;
        }
        flags->error = 1;
      }
    } else if (flags->e == 1 || (no_e(argv, argc) == 1 && *w_cu == 0)) {
      buf = strlen(argv[i]);
      if (*w_cu == 0 || compare(argv[i], words, *w_cu) != 0) {
        for (int k = 0; k < buf; k++) words[*w_cu][k] = argv[i][k];
        (*w_cu)++;
      }
      flags->e = 0;
    } else {
      buf = strlen(argv[i]);
      for (int k = 0; k < buf; k++) file_names[*f_cu][k] = argv[i][k];
      (*f_cu)++;
    }
  }
}

void show_file(struct Flags *flags, char file_names[256][256],
               char words[256][256], int *w_cu, int *f_cu) {
  FILE *fp = NULL;
  int c = 0, l = 0;
  char Lfiles[256][256] = {0};
  for (int i = 0; i < (*f_cu); i++) {
    if ((fp = fopen(file_names[i], "r")) == NULL) {
      printf("grep:\t%s:No such file or directory\n", file_names[i]);
    } else {
      with_flags(flags, fp, words, *w_cu, file_names[i], *f_cu, &c, &l, Lfiles);
      fclose(fp);
    }
  }
  if (flags->l == 1) {
    for (int i = 0; i < l; i++) printf("%s\n", Lfiles[i]);
  }
}

void with_flags(struct Flags *flags, FILE *fp, char words[256][256], int w_cu,
                char *f, int f_cu, int *c, int *l, char Lfiles[256][256]) {
  char line[2048] = {0};
  int num = 0, stop = 0;
  while (fgets(line, 2048, fp) != NULL) {
    num++;
    if (flags->c == 1) {
      count_str(w_cu, line, words, c);
      continue;
    }
    if (flags->i == 1) {
      print_with_i(w_cu, line, words, f_cu, f);
      continue;
    }
    if (flags->v == 1) {
      print_with_v(w_cu, line, words, f_cu, f);
      continue;
    }
    if (flags->l == 1 && stop == 0) {
      if (print_with_l(w_cu, line, words) == 1) {
        strcpy(Lfiles[(*l)++], f);
        stop = 1;
      }
      continue;
    }
    if (flags->n == 1) {
      print_with_n(w_cu, line, words, f_cu, f, num);
      continue;
    } else if (stop == 0) {
      print_without_flags(w_cu, line, words, f_cu, f);
    }
  }
  if (flags->c == 1) {
    print_with_c(f_cu, f, *c);
    (*c) = 0;
  }
}

void print_with_v(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f) {
  for (int i = 0; i < w_cu; i++) {
    if (strstr(line, words[i]) == NULL) {
      if (f_cu == 1) {
        show(line);
      } else {
        printf("%s:", f);
        show(line);
      }
    }
  }
}

void print_with_n(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f, int num) {
  for (int i = 0; i < w_cu; i++) {
    if (strstr(line, words[i]) != NULL) {
      if (f_cu == 1) {
        printf("%d:", num);
        show(line);
      } else {
        printf("%s:%d:", f, num);
        show(line);
      }
    }
  }
}

void print_with_c(int f_cu, char *f, int c) {
  if (f_cu == 1)
    printf("%d\n", c);
  else
    printf("%s:%d\n", f, c);
}

int print_with_l(int w_cu, char line[2048], char words[256][256]) {
  int find = 0;
  for (int i = 0; i < w_cu && find == 0; i++) {
    if (strstr(line, words[i]) != NULL) {
      find = 1;
    }
  }
  return find;
}

void print_with_i(int w_cu, char line[2048], char words[256][256], int f_cu,
                  char *f) {
  char line_buf[2048] = {0};
  char word_buf[2048] = {0};
  int flag = 0, j;
  for (int i = 0; line[i]; i++) {
    line_buf[i] = line[i];
    to_lower(line_buf);
  }
  for (int i = 0; i < w_cu; i++) {
    for (j = 0; words[i][j]; j++) {
      word_buf[j] = words[i][j];
      to_lower(word_buf);
    }
    if (strstr(line_buf, word_buf) != NULL && flag == 0) {
      if (f_cu == 1) {
        show(line);
      } else {
        printf("%s:", f);
        show(line);
      }
      flag = 1;
    }
    for (j = 0; word_buf[j]; j++) word_buf[j] = 0;
  }
  for (int i = 0; line[i]; i++) line_buf[i] = 0;
}

void print_without_flags(int w_cu, char line[2048], char words[256][256],
                         int f_cu, char *f) {
  for (int i = 0; i < w_cu; i++) {
    if (strstr(line, words[i]) != NULL) {
      if (f_cu == 1) {
        show(line);
      } else {
        printf("%s:", f);
        show(line);
      }
    }
  }
}

void show(char line[2048]) {
  int c = 0;
  for (int i = 0; line[i]; i++, c++) printf("%c", line[i]);
  if (c > 0 && line[--c] != '\n') {
    printf("\n");
  }
}

void count_str(int w_cu, char line[2048], char words[256][256], int *c) {
  for (int i = 0; i < w_cu; i++) {
    if (strstr(line, words[i]) != NULL) (*c)++;
  }
}

void to_lower(char str[2048]) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
}

int compare(char argv[], char massive[256][256], int cu) {
  int flag = 1;
  for (int i = 0; i < cu && flag == 1; i++) {
    if (strcmp(argv, massive[i]) == 0) flag = 0;
  }
  return flag;
}

int no_e(char *argv[], int argc) {
  int check = 1;
  char buf[3] = "-e";
  for (int i = 0; i < argc; i++) {
    if (strcmp(buf, argv[i]) == 0) {
      check = 0;
    }
  }
  return check;
}
