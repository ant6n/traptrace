#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "main.h"


static char* join_strings(char* const key, char* const value) {
  char* s = malloc(strlen(key) + strlen(value) + 2);
  sprintf(s, "%s=%s", key, value);
  return s;
}

static int assert_has_file(int index, int argc, char* const argv[]) {
  if (index + 1 >= argc) {
    fprintf(stderr, "missing argument for parameter %s!", argv[index]);
    exit(EXIT_FAILURE);
  }
}


static int env_len(char *const envp[]) {
  int num_env = -1;
  while (envp[++num_env] != NULL);
  return num_env;
}

static char **const merge_envp(char *const envp1[], char *const envp2[]) {
  int num_env1 = env_len(envp1);
  int num_env2 = env_len(envp2);
  
  char** result = malloc(sizeof(char *const)*(num_env1 + num_env2 + 1));
  int i = 0;
  while (envp1[i] != NULL) {
    result[i] = envp1[i];
    i++;
  }
  int j = 0;
  while (envp2[j] != NULL) {
    result[i] = envp2[j];
    i++;
    j++;
  }
  result[i] = NULL;
  return result;
}


#define LINE_LENGTH 2000
static int has_libc_start_main(char* program) {
char* cmd = malloc(sizeof(char)*(strlen(program) + 20));
  sprintf(cmd, "nm -D %s", program);
  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    printf("cannot analyze program: %s\n", program);
    exit(EXIT_FAILURE);
  }

  char line[LINE_LENGTH];
  int found = 0;
  while (fgets(line, LINE_LENGTH, fp)) {
    if (strstr(line, LIBC_START_MAIN) != NULL) {
      found = 1;
    }
  }
  pclose(fp);
  return found;
}



int main(int argc, char *const argv[], char *const envp1[]) {
  const int NUM_ENVP_EXTRA = 10;
  char** envp2 = malloc(sizeof(char*)*NUM_ENVP_EXTRA);
  int env2_index = 0;
  
  int i = 1;
  while (i < argc) {
    const char* s = argv[i];
    int len = strlen(s);
    
    if (strcmp(s, "-h")==0 || strcmp(s, "--help")==0) {
      printf("Help:  trap-trace\n"
             "traces through a program step by step, starting at __lib_start_main\n"
             "arguments:\n"
             " -h, --help:                       print this help\n"
             " -v, --version:                    print version\n"
             " -s <file>, --syscalls <file>:     store system calls in <file>, use - for stdout\n"
             " -i <file>, --instructions <file>: store every encountered instruction in <file>, - for stdout\n"
             " --stats <file>:                   store instruction stats in file, - for stdout\n");
      exit(EXIT_SUCCESS);
    }
    if ((strcmp(s, "-v")==0) || (strcmp(s, "--version")==0)) {
      printf("traptrace v%s\n", VERSION);
      exit(EXIT_SUCCESS);
    }
    
    if ((strcmp(s, "-s")==0) || (strcmp(s, "--syscalls")==0)) {
      assert_has_file(i, argc, argv);
      envp2[env2_index++] = join_strings(SYSCALLS_FILE, argv[i+1]);
      i += 1;
    } else if ((strcmp(s, "-i")==0) || (strcmp(s, "--instructions")==0)) {
      assert_has_file(i, argc, argv);
      envp2[env2_index++] = join_strings(INSTRUCTIONS_FILE, argv[i+1]);
      i += 1;
    } else if ((strcmp(s, "--stats")==0)) {
      assert_has_file(i, argc, argv);
      envp2[env2_index++] = join_strings(STATS_FILE, argv[i+1]);
      i += 1;
    } else if (s[0] == '-') {
      printf("unkown option %s\n", s);
      exit(EXIT_FAILURE);
    } else {
      break;
    }
    
    i += 1;
  }

  char* program = argv[i];
  if (!has_libc_start_main(program)) {
    printf("program '%s' does not have main to override (%s)\n", program, LIBC_START_MAIN);
    exit(EXIT_FAILURE);
  }
  
  envp2[env2_index++] = join_strings("LD_PRELOAD", OVERRIDE_LIB);
  char**const new_envp = merge_envp(envp1, envp2);
  
  printf("[execute '%s']\n", program);
  if (execve(program, argv + i, new_envp) < 0) {
    printf("executing '%s' failed: ", argv[i]);
    switch (errno) {
    case E2BIG: printf("E2BIG"); break;
    case EACCES: printf("EACCES"); break;
    case EFAULT: printf("EFAULT"); break;
    case EIO: printf("EIO"); break;
    case EISDIR: printf("EISDIR"); break;
    case ELIBBAD: printf("ELIBBAD"); break;
    case ELOOP: printf("ELOOP"); break;
    case EMFILE: printf("EMFILE"); break;
    case ENAMETOOLONG: printf("ENAMETOOLONG"); break;
    case ENFILE: printf("ENFILE"); break;
    case ENOENT: printf("ENOENT"); break;
    case ENOEXEC: printf("ENOEXEC"); break;
    case ENOMEM: printf("ENOMEM"); break;
    case ENOTDIR: printf("ENOTDIR"); break;
    case EPERM: printf("EPERM"); break;
    case ETXTBSY: printf("ETXTBSY"); break;
    default:
      printf("unknown error");
    }
    printf("\n");
    exit(EXIT_FAILURE);
  }
}
