/* Jeffrin Daniel, 118442853 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include "command.h"
#include "executor.h"

#define SUCCESS 0
#define FAILURE 1

int execute(struct tree *t);
int execute_aux(struct tree *t, int input_fd, int output_fd);
static void print_tree(struct tree *t);

int execute(struct tree *t) {
  /* print_tree(t); */

  return execute_aux(t, STDIN_FILENO, STDOUT_FILENO);
}

int execute_aux(struct tree *t, int input_fd, int output_fd) {
  pid_t pid, pid2, pid3, pid4;
  int status, status2, status3, AND_status, SUB_status, current_input, 
    current_output, pipes[2];

  if (t != NULL) {

    if (t->input != NULL) {
      if ((current_input = open(t->input, O_RDONLY)) < 0) {
	perror("Cannot open file");
      }
      if (dup2(current_input, input_fd) < 0) {
	perror("dup2 error");
      }
      if (close(current_input) < 0) {
	perror("close error");
      }
    }
    if (t->output != NULL) {
      if ((current_output = open(t->output, O_WRONLY | O_CREAT | O_TRUNC, 0664)) < 0) {
	perror("File opening (write) failed");
      }
      if (dup2(current_output, output_fd) < 0) {
	perror("dup2 error");
      }
      if (close(current_output) < 0) {
	perror("close error");
      }
    }

    /* NONE conjunction */
    if (t->conjunction == NONE) {
      /* checks for the cd command*/
      if (strcmp(t->argv[0], "cd") == 0) {
	if (t->argv[1] == NULL) {
	  t->argv[1] = getenv("HOME");
	}
	if (chdir(t->argv[1]) == -1) {
	  perror(t->argv[1]);
	  return FAILURE;
	}
      } else if (strcmp(t->argv[0], "exit") == 0) {
	exit(0);
      } else {
	
	if ((pid = fork()) < 0) {
	  perror("fork error");
	}
	
	/* Parent */
	if (pid != 0) {
	  wait(&status);
	  
	  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
	    return SUCCESS;
	  } else {
	    return FAILURE;
	  }
	  
	/* Child */
	} else {
	  execvp(t->argv[0], t->argv);
	  fprintf(stderr, "Failed to execute %s\n", t->argv[0]);
	  exit(FAILURE);
	}
      }

      /* AND conjunction*/
    } else if (t->conjunction == AND) {
      if ((AND_status = execute_aux(t->left, input_fd, output_fd)) == SUCCESS) {
	return execute_aux(t->right, input_fd, output_fd);
      } else {
	return AND_status;
      }

      /* PIPE conjunction */
    } else if (t->conjunction == PIPE) {
      if (t->left->output != NULL) {
	printf("Ambiguous output redirect.\n");
	return FAILURE;
      }
      if (t->right->input != NULL) {
	printf("Ambiguous input redirect.\n");
	return FAILURE;
      }

      if (pipe(pipes) < 0) {
	perror("pipe error");
      }
      if ((pid2 = fork()) < 0) {
	perror("fork error");
      }
      /* Parent */
      if (pid2 != 0) {

	/* Child 2 within parent code */
	if ((pid3 = fork()) < 0) {
	  perror("fork error");
	}
	/* Parent (Child 2)*/
	if (pid3 != 0) {
	  close(pipes[0]);
	  close(pipes[1]);

	  wait(&status2);
	  wait(&status3);

	  if ((WIFEXITED(status2) && WEXITSTATUS(status2) == 0) &&
	      (WIFEXITED(status3) && WEXITSTATUS(status3) == 0)) {
	    return SUCCESS;
	  } else {
	    return FAILURE;
	  }
	
	/* Child of Child 2 */
	} else {
	  close(pipes[1]);
	  
	  if (dup2(pipes[0], input_fd) < 0) {
	    perror("dup2 error");
	  }
	  close(pipes[0]);
	  execute_aux(t->right, input_fd, output_fd);
	  exit(SUCCESS);
	}

      /* Child 1 */
      } else {
	close(pipes[0]);

	if (dup2(pipes[1], output_fd) < 0) {
	  perror("dup2 error");
	}
	close(pipes[1]);
	execute_aux(t->left, input_fd, output_fd);
	exit(SUCCESS);
      }

      /* SUBSHELL conjunction */
    } else if (t->conjunction == SUBSHELL) {
      if ((pid4 = fork()) < 0) {
	perror("fork error");
      }
      /* Parent */
      if (pid4 != 0) {
	wait(&SUB_status);

     	if (WIFEXITED(SUB_status) && WEXITSTATUS(SUB_status) == 0) {
	  return SUCCESS;
	} else {
	  return FAILURE;
	}
       

      /* Child */
      } else {
	execute_aux(t->left, input_fd, output_fd);
	return FAILURE;
      }
    }
  }
  return SUCCESS;
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

