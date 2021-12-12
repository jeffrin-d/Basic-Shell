/* Jeffrin Daniel, 118442853 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "command.h"
#include "executor.h"

static void print_tree(struct tree *t);

int execute(struct tree *t) {
  execute_aux(t, STDIN_FILENO, STDOUT_FILENO);

  print_tree(t);

  return 0;
}

execute_aux(struct tree *t, int input_fd, int output_fd) {
  if (t->conjunction == NONE) {
    /* checks for the cd command*/
    if (strcmp(t->argv[0], "cd") == 0) {
      if (!t->argv[1]) {
	t->argv[1] = getenv("HOME");
      }
      if (chdir(t->argv[1]) == -1) {
	perror(t->argv[1]);
      }
    }
  } else if (t->conjunction
}

static void print_tree(struct tree *t) {
   if (t != NULL) {
      print_tree(t->left);

      if (t->conjunction == NONE) {
         printf("NONE: %s, ", t->argv[0]);
      } else {
         printf("%s, ", conj[t->conjunction]);
      }
      printf("IR: %s, ", t->input);
      printf("OR: %s\n", t->output);

      print_tree(t->right);
   }
}

