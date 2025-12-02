
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 80
#define PATH_MAX 1024
#define MAX_JOBS 5
#define HISTORY_SIZE 50

// defining necessary global variables
static char prev_dir[PATH_MAX] = "";
static char history[HISTORY_SIZE][BUFFER_SIZE];
static int history_count = 0;
static volatile sig_atomic_t fg_pid = -1;

// defining job structure
typedef struct
{
  pid_t pid;
  char command[BUFFER_SIZE];
  int running; // 1 if active, 0 if finished
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

// defining structure to hold all of our built-ins
typedef struct
{
  const char *name;
  int (*ptr)(char **);
} builtin;

extern builtin array[];

int my_cd(char **args)
{
  // first check if the right number of arguments were inputted
  if (args[1] != NULL && args[2] != NULL)
  {
    fprintf(stderr, "Too many arguments.\n");
    return -1;
  }

  // create a current directory array and get the present dir
  char current_dir[PATH_MAX];
  if (getcwd(current_dir, sizeof(current_dir)) == NULL)
  {
    perror("cd");
    return -1;
  }

  // handle cases of cd or cd ~ which will go tothe home
  if (args[1] == NULL || strcmp(args[1], "~") == 0)
  {
    char *home = getenv("HOME");
    // if error getting home set it to the root directory
    if (home == NULL)
    {
      home = "/";
    }
    // change to home directory and handle error if unable to
    if (chdir(home) != 0)
    {
      perror("cd");
      return -1;
    }
  }
  // Handle case of cd - which goes to previous directory
  else if (strcmp(args[1], "-") == 0)
  {
    // check if previous directory exists
    if (prev_dir[0] == '\0')
    {
      fprintf(stderr, "No previous directory\n");
      return -1;
    }
    // change to prev dir and handle error if unable to
    if (chdir(prev_dir) != 0)
    {
      perror("cd");
      return -1;
    }
    // print out the directory
    printf("%s\n", prev_dir);
  }
  // handles the normal case where cd is printing with the name of directory
  else
  {
    // changes to the inputted directory and handles error if not
    if (chdir(args[1]) != 0)
    {
      perror("cd");
      return -1;
    }
  }
  // copies over prev to current
  // works for cd - which toggles when u keep typing it
  strcpy(prev_dir, current_dir);
  return 0;
}

int my_exit(char **args)
{
  printf("Exiting shell\n");

  // makes sure to clean up background jobs
  for (int i = 0; i < job_count; i++)
  {
    if (jobs[i].running)
    {
      kill(jobs[i].pid, SIGTERM);
      jobs[i].running = 0;
    }
  }
  // call exit
  exit(0);
}

int my_help(char **args)
{
  printf("Amy's mini-shell\n");
  printf("Built-in Commands:\n");
  // loops through and names our built-ins
  for (int i = 0; array[i].name != NULL; i++)
  {
    printf("  %s\n", array[i].name);
  }
  return 0;
}

int my_history(char **args)
{
  // simply calls on the history array and prints it out
  for (int i = 0; i < history_count; i++)
  {
    printf("%d  %s\n", i + 1, history[i]);
  }
  return 0;
}

int my_jobs(char **args)
{
  // simply calls on the job array and prints it out
  for (int i = 0; i < job_count; i++)
  {
    if (jobs[i].running)
    {
      printf("[%d]  Stopped     %s\n", i + 1, jobs[i].command);
    }
  }
  return 0;
}

int my_fg(char **args)
{
  int job_num;

  // if there are no arguments look for most recent job
  if (args[1] == NULL)
  {
    job_num = -1;
    for (int i = job_count - 1; i >= 0; i--)
    {
      if (jobs[i].running)
      {
        job_num = i + 1;
        break;
      }
    }
    if (job_num == -1)
    {
      fprintf(stderr, "fg: no job to foreground\n");
      return -1;
    }
  }
  else
  {
    // covert the string to int
    job_num = atoi(args[1]);
    // check against invalid number input
    if (job_num <= 0 || job_num > job_count)
    {
      fprintf(stderr, "fg: invalid job number\n");
      return -1;
    }
  }

  // check if job is active
  Job *job = &jobs[job_num - 1];
  if (!job->running)
  {
    fprintf(stderr, "fg: job %d is not active\n", job_num);
    return -1;
  }
  printf("%s\n", job->command);

  // this is one part of the pset that i had a lot of trouble with implementing
  // for my own understanding, i will try to be verbose with my comments

  // in UNIX shells the foreground process group is the one that has control over the terminal
  // this means that when resuming a job from background, we must give it terminal ownership

  // give the terminal to job and resume it
  // isatty checks if stdin is connected to a terminal
  if (isatty(STDIN_FILENO))
  {
    // changes the foreground process group of the terminal to have the jobs pid
    tcsetpgrp(STDIN_FILENO, job->pid);
  }

  // actually resumes the job
  fg_pid = job->pid;
  kill(job->pid, SIGCONT);

  // wait for the job to finish or stop again
  int status;
  waitpid(job->pid, &status, WUNTRACED);

  // VERY IMPORTANT!!! returns ownership
  // then take back the terminal once job ends
  if (isatty(STDIN_FILENO))
  {
    tcsetpgrp(STDIN_FILENO, getpid());
  }
  // no longer a foreground process
  fg_pid = -1;

  // checks if the user stops the job again
  if (WIFSTOPPED(status))
  {
    printf("\n[Job %d] PID=%d stopped again\n", job_num, job->pid);
    return 0;
  }

  // this is when the job is (finally....) finished
  job->running = 0;
  printf("Job [%d] PID=%d finished\n", job_num, job->pid);
  return 0;
}

int my_bg(char **args)
{
  int job_num;

  // if no args are given find the most recent job
  if (args[1] == NULL)
  {
    job_num = -1;
    for (int i = job_count - 1; i >= 0; i--)
    {
      if (jobs[i].running)
      {
        job_num = i + 1;
        break;
      }
    }
    if (job_num == -1)
    {
      fprintf(stderr, "bg: no jobs to background\n");
      return -1;
    }
  }
  else
  {
    // covert the string to int
    job_num = atoi(args[1]);
    // check if valid int input
    if (job_num <= 0 || job_num > job_count)
    {
      fprintf(stderr, "bg: invalid job number\n");
      return -1;
    }
  }

  // check if job is active
  Job *job = &jobs[job_num - 1];
  if (!job->running)
  {
    fprintf(stderr, "bg: job %d is not active\n", job_num);
    return -1;
  }

  // if active resume the job
  printf("bg: resuming job [%d] %s\n",
         job_num, job->command);

  kill(job->pid, SIGCONT);
  return 0;
}

void sigint_handler(int sig)
{
  if (fg_pid > 0)
  {
    // this will forward ctrl c to foreground process
    kill(fg_pid, SIGINT);
  }
  else
  {
    // redisplay the prompt afterward to continue shell use
    write(STDOUT_FILENO, "\nmini-shell> ", 13);
    fflush(stdout);
  }
}

void sigtstp_handler(int sig)
{
  if (fg_pid > 0)
  {
    // this will forward ctrl z to foreground process
    kill(fg_pid, SIGTSTP);
  }
}

// needed to convert from dynamicstring_t to argv array
char **change_argv(dynamicstring_t **tokens, int count)
{
  char **argv = malloc(sizeof(char *) * (count + 1));

  // check malloc allocation
  if (argv == NULL)
  {
    return NULL;
  }

  // loop through using strdup
  for (int j = 0; j < count; j++)
  {
    argv[j] = strdup(tokens[j]->buf);
    DynamicString_Free(tokens[j]);
  }
  argv[count] = NULL;
  // free the dynamicstrings
  free(tokens);
  return argv;
}

// makes sure we have no zombie processess!
void no_zombies(void)
{
  // loop thorugh jobs
  for (int i = 0; i < job_count; i++)
  {
    // if there is an active job
    if (jobs[i].running)
    {
      int status;
      // if job is still running returns 0
      // if job is finished returns pid
      // if error returns -1
      pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
      // this means process has finished
      // mark it as no longer running
      if (result > 0)
      {
        jobs[i].running = 0;
        printf("job %d finished\n", i + 1);
      }
    }
  }
}

// handles pipe commands
int pipe_func(char **argv, int pipe_count)
{
  // create array that seperates the command into pipe sections
  char **sections[pipe_count + 1];
  int start = 0;
  int arg_start = 0;

  // loop through while we have args
  for (int i = 0; argv[i] != NULL; i++)
  {
    // if we get to a pipe replace it with null terminator
    if (strcmp(argv[i], "|") == 0)
    {
      argv[i] = NULL;
      sections[start++] = &argv[arg_start];
      arg_start = i + 1;
    }
  }
  // handle last command
  sections[start] = &argv[arg_start];

  // initalize necessary counts and arrays
  int num_cmds = pipe_count + 1;
  int fd[pipe_count][2];
  pid_t pids[num_cmds];

  // create the pipes based of our number of pipees
  for (int i = 0; i < pipe_count; i++)
  {
    // check for error
    if (pipe(fd[i]) == -1)
    {
      perror("pipe");
      return -1;
    }
  }

  // go through each command and fork
  for (int i = 0; i < num_cmds; i++)
  {
    pid_t pid = fork();

    // check for error
    if (pid < 0)
    {
      perror("fork");
      return -1;
    }
    // if were are at child
    else if (pid == 0)
    {
      // the child restores default signals
      signal(SIGINT, SIG_DFL);
      signal(SIGTSTP, SIG_DFL);

      // connects the pipes
      // if it isnt the first command
      if (i > 0)
      {
        // read input from previous pipe's command
        dup2(fd[i - 1][0], STDIN_FILENO);
      }
      // if it isnt last command
      if (i < pipe_count)
      {
        // write output into the current pipes write end
        dup2(fd[i][1], STDOUT_FILENO);
      }

      // close all of the unused pipes
      for (int j = 0; j < pipe_count; j++)
      {
        close(fd[j][0]);
        close(fd[j][1]);
      }
      // execute the commands
      execvp(sections[i][0], sections[i]);
      perror("execvp failed");
      exit(EXIT_FAILURE);
    }
    // store child pid for later waitpid
    else
    {
      pids[i] = pid;
    }
  }

  // close all pipes in parents
  // parent does not participate in pipeline..close everything!!
  for (int i = 0; i < pipe_count; i++)
  {
    close(fd[i][0]);
    close(fd[i][1]);
  }

  // wait for all children
  int last_status = 0;
  for (int i = 0; i < num_cmds; i++)
  {
    // store last commands exit status
    int status;
    waitpid(pids[i], &status, 0);
    if (i == num_cmds - 1)
    {
      // if last command finished normally, return its code
      // if killed by signal, encode it
      // else error
      if (WIFEXITED(status))
      {
        last_status = WEXITSTATUS(status);
      }
      else if (WIFSIGNALED(status))
      {
        last_status = 128 + WTERMSIG(status);
      }
      else
      {
        last_status = 1;
      }
    }
  }
  return last_status;
}

// this executes a single command or pipeline
int run_segment(char **segment_argv)
{
  // check for NULL input
  if (segment_argv == NULL || segment_argv[0] == NULL)
  {
    return 0;
  }

  // loop thru to count the number of pipes
  int pipe_count = 0;
  for (int i = 0; segment_argv[i] != NULL; i++)
  {
    if (strcmp(segment_argv[i], "|") == 0)
    {
      pipe_count++;
    }
  }
  // if we have pipe input send it to pipe function
  if (pipe_count > 0)
  {
    return pipe_func(segment_argv, pipe_count);
  }

  // next we check for built-ins
  for (int i = 0; array[i].name != NULL; i++)
  {
    // loop thru with strcmp on input and builtin array
    if (strcmp(segment_argv[0], array[i].name) == 0)
    {
      int result = array[i].ptr(segment_argv);

      if (result == 0)
      {
        // success
        return 0;
      }
      else
      {
        // error
        return 1;
      }
    }
  }

  // finally execute the external command since we now know it is not a builtin
  pid_t pid = fork();
  if (pid == 0)
  {
    // create a new process group
    setpgid(0, 0);

    // gives terminal control to this new child
    if (isatty(STDIN_FILENO))
    {
      tcsetpgrp(STDIN_FILENO, getpid());
    }

    // restore defasult signs so that interuppts effect the child and not the shell
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    // execute the command and handle error input
    execvp(segment_argv[0], segment_argv);
    fprintf(stderr, "Command not found--Did you mean something else?\n");
    _exit(127);
  }

  // the parent gives terminal to child and waits
  setpgid(pid, pid);
  if (isatty(STDIN_FILENO))
  {
    tcsetpgrp(STDIN_FILENO, pid);
  }

  // track the foreground process and wait for it exit or stop
  fg_pid = pid;
  int status;
  waitpid(pid, &status, WUNTRACED);

  // take back the terminal
  if (isatty(STDIN_FILENO))
  {
    tcsetpgrp(STDIN_FILENO, getpid());
  }
  fg_pid = -1;

  // check for ctrl z
  // if job is stopped
  if (WIFSTOPPED(status))
  {
    // and we have not reached max jobs yet
    if (job_count < MAX_JOBS)
    {
      // add to jobs list
      jobs[job_count].pid = pid;
      strncpy(jobs[job_count].command, segment_argv[0], BUFFER_SIZE - 1);
      jobs[job_count].command[BUFFER_SIZE - 1] = '\0';
      jobs[job_count].running = 1;
      // printf("\n[Job %d] stopped  pid = %d  %s\n", job_count + 1, pid, jobs[job_count].command);
      job_count++;
    }
    // arb value code to return
    return 999;
  }

  // handles normal termination
  // if there was a normal exit return the exit code
  if (WIFEXITED(status))
  {
    return WEXITSTATUS(status);
  }
  // if interuppted encode 128 + signal number
  if (WIFSIGNALED(status))
  {
    return 128 + WTERMSIG(status);
  }
  return 1;
}

// handles the logical operators && || ;
int operators(char **argv)
{
  int i = 0,
      start = 0;
  int last_status = 0;
  const char *prev_op = NULL;

  while (argv[i] != NULL)
  {
    // find the next operator
    while (argv[i] != NULL &&
           strcmp(argv[i], "&&") != 0 &&
           strcmp(argv[i], "||") != 0 &&
           strcmp(argv[i], ";") != 0)
    {
      i++;
    }
    // marks the segment end
    char *op = argv[i];
    if (op != NULL)
    {
      argv[i] = NULL;
    }

    // decide if it should run
    int run = 1;
    if (prev_op != NULL)
    {
      // implement and or logic
      if (strcmp(prev_op, "&&") == 0)
      {
        run = (last_status == 0);
      }
      else if (strcmp(prev_op, "||") == 0)
      {
        run = (last_status != 0);
      }
      // always run ;
    }
    // call run_segment if we decide it should run
    if (argv[start] != NULL && run)
    {
      last_status = run_segment(&argv[start]);
    }

    // break if weve reached end
    if (op == NULL)
    {
      break;
    }
    // otherwise advance to parse next segment
    prev_op = op;
    i++;
    start = i;
  }
  // return status
  return last_status;
}

// launches processes
int launch_fork(char **argv, int background)
{
  pid_t pid = fork();
  // we're in child process
  if (pid == 0)
  {
    // set up process group
    setpgid(0, 0);

    // if running in foreground give terminal control to new process
    if (!background && isatty(STDIN_FILENO))
    {
      tcsetpgrp(STDIN_FILENO, getpid());
    }

    // restores default signal behavior
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    // execute the command and handle error if unable
    execvp(argv[0], argv);
    fprintf(stderr, "Command not found--Did you mean something else?\n");
    exit(1);
  }
  // we're in parent process
  else if (pid > 0)
  {

    setpgid(pid, pid);

    // if there is background job
    if (background)
    {
      // and we havent reached max jobs
      if (job_count < MAX_JOBS)
      {
        // add to job array
        jobs[job_count].pid = pid;
        strcpy(jobs[job_count].command, argv[0]);
        jobs[job_count].running = 1;
        job_count++;
        printf("[job %d] running in background\n", job_count);
      }
      else
      {
        fprintf(stderr, "Too many background jobs!\n");
      }
    }
    else
    {
      // foreground job
      if (isatty(STDIN_FILENO))
      {
        tcsetpgrp(STDIN_FILENO, pid);
      }

      // waits until process finishes or is interuppted
      fg_pid = pid;
      int status;
      waitpid(pid, &status, WUNTRACED);

      // once child is done return control to shell
      if (isatty(STDIN_FILENO))
      {
        tcsetpgrp(STDIN_FILENO, getpid());
      }

      // check if it is stopped
      if (WIFSTOPPED(status))
      {
        if (job_count < MAX_JOBS)
        {
          // record in job list for fg/bg later
          jobs[job_count].pid = pid;
          strncpy(jobs[job_count].command, argv[0], BUFFER_SIZE - 1);
          jobs[job_count].command[BUFFER_SIZE - 1] = '\0';
          jobs[job_count].running = 1;
          printf("\n[Job %d] stopped %s\n", job_count + 1, jobs[job_count].command);
          job_count++;
        }
      }
      fg_pid = -1;
    }
  }
  return 0;
}

// create array with our built-in commands
builtin array[] = {
    {.name = "cd", .ptr = my_cd},
    {.name = "exit", .ptr = my_exit},
    {.name = "fg", .ptr = my_fg},
    {.name = "jobs", .ptr = my_jobs},
    {.name = "bg", .ptr = my_bg},
    {.name = "help", .ptr = my_help},
    {.name = "history", .ptr = my_history},
    {.name = NULL},
};

int main(int argc, char **argv)
{
  // alarm against fork bombs
  alarm(120);

  // set up our shell process group and terminal control
  if (isatty(STDIN_FILENO))
  {
    pid_t shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0)
    {
      perror("setpgid");
      exit(1);
    }
    tcsetpgrp(STDIN_FILENO, shell_pgid);
  }

  // set up our signal handlers
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  char str[BUFFER_SIZE];

  // our infitnte loop that is the essence of our shell
  while (1)
  {
    printf("mini-shell> ");
    fflush(stdout);

    // reads in input using fgets
    errno = 0;
    if (fgets(str, BUFFER_SIZE, stdin) == NULL)
    {
      // handles case of EOF
      if (feof(stdin))
      {
        putchar('\n');
        break;
      }
      // hand;es interuppted
      if (errno == EINTR)
      {
        putchar('\n');
        clearerr(stdin);
        continue;
      }
      // other read errors
      clearerr(stdin);
      continue;
    }

    // removes the trailing newlines
    str[strcspn(str, "\n")] = '\0';

    // update our history array
    if (strlen(str) > 0 && history_count < HISTORY_SIZE)
    {
      strncpy(history[history_count++], str, BUFFER_SIZE - 1);
      history[history_count - 1][BUFFER_SIZE - 1] = '\0';
    }

    // parse the input using our parser
    int count;
    dynamicstring_t **tokens = run_parser((char *[]){"parser", str}, &count);

    // continue if we have no input
    if (count == 0)
    {
      continue;
    }
    // conveert to char**
    char **my_argv = change_argv(tokens, count);

    // NULL check
    if (my_argv == NULL || my_argv[0] == NULL)
    {
      if (my_argv != NULL)
        free(my_argv);
      continue;
    }

    // checks if there is a background &
    int background = 0;
    if (count > 0 && strcmp(my_argv[count - 1], "&") == 0)
    {
      background = 1;
      free(my_argv[count - 1]);
      my_argv[count - 1] = NULL;
      count--;
    }

    // handle case of no input
    if (my_argv[0] == NULL)
    {
      fprintf(stderr, "Invalid command\n");
      free(my_argv);
      continue;
    }

    // check for logical operators && || ;
    int logic = 0;
    for (int i = 0; my_argv[i] != NULL; i++)
    {
      if (strcmp(my_argv[i], "&&") == 0 ||
          strcmp(my_argv[i], "||") == 0 ||
          strcmp(my_argv[i], ";") == 0)
      {
        logic = 1;
        break;
      }
    }

    if (logic)
    {
      // send to operators func
      operators(my_argv);
    }
    else
    {
      // single command checking for pipes first
      int has_pipe = 0;
      for (int i = 0; my_argv[i] != NULL; i++)
      {
        if (strcmp(my_argv[i], "|") == 0)
        {
          has_pipe = 1;
          break;
        }
      }

      // if there is a pipe call run_segment
      if (has_pipe)
      {
        run_segment(my_argv);
      }
      else
      {
        // check for built-ins
        int found = 0;
        for (int i = 0; array[i].name != NULL; i++)
        {
          if (strcmp(my_argv[0], array[i].name) == 0)
          {
            array[i].ptr(my_argv);
            found = 1;
            break;
          }
        }

        // if its not a builtin launch the external command
        if (!found)
        {
          launch_fork(my_argv, background);
        }
      }
    }

    // memory clean up
    for (int j = 0; j < count; j++)
    {
      if (my_argv[j] != NULL)
      {
        free(my_argv[j]);
      }
    }
    free(my_argv);

    // take care of zombies
    no_zombies();
  }

  return 0;
}