#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "commands.h"
#include "built_in.h"
#include "signal_handlers.h"

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }
  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
	exit(1);
      //return 1;
    } else {
	pid_t pID = fork();
	int status1, status2, status3, status4, status5;
	//char* cmd[] = {"ls", "-F", NULL};
	char buffer[100] = "/bin/";
	char buffer2[100] = "/usr/bin/";
	char buffer3[100] = "/sbin/";
	char buffer4[100] = "/usr/sbin/";
	char* ptr, *ptr2, *ptr3, *ptr4;
	ptr = strcat(buffer, com->argv[0]);
	ptr2 = strcat(buffer2, com->argv[0]);
	ptr3 = strcat(buffer3, com->argv[0]);
	ptr4 = strcat(buffer4, com->argv[0]);
	switch(pID){
	case 0:{ //child
	status1 = execv(com->argv[0], com->argv);
	status2 = execv(ptr, com->argv);
	status3 = execv(ptr2, com->argv);
	status4 = execv(ptr3, com->argv);
	status5 = execv(ptr4, com->argv);
	if(status1 == -1 | status2 == -1 | status3 == -1| status4 == -1 | status5 == -1){
	 perror("execv failed");
	 break;
	}
	}
	case -1:{
	break;
	}
	default:{ //parent
	wait((int*)0);
	break;
	}
	} 
	
/*      if(pID == 0){    			//child process 
	execv(com->argv[0], com->argv);

	perror("execve failed");
	return -1;
      }else if(pID > 0){
	wait((int*)0);
	return -1;
      }
      else{		
	perror("fork failed");
	return -1;	
      }
*/
    }
  }
  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
