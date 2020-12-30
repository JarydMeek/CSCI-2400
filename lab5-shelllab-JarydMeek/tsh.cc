//
// tsh - A tiny shell program with job control
//
// John Korhel joko3539
// Jaryd Meek jame7931
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "globals.h"
#include "jobs.h"
#include "helper-routines.h"

using namespace std;

//
// Needed global variable definitions
//

static char prompt[] = "tsh> ";
int verbose = 0;

extern char **environ;

//
// You need to implement the functions eval, builtin_cmd, do_bgfg,
// waitfg, sigchld_handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
//

void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

//
// main - The shell's main routine
//
int main(int argc, char *argv[])
{
  int emit_prompt = 1; // emit prompt (default)

  //
  // Redirect stderr to stdout (so that driver will get all output
  // on the pipe connected to stdout)
  //
  dup2(1, 2);

  /* Parse the command line */
  char c;
  while ((c = getopt(argc, argv, "hvp")) != EOF)
  {
    switch (c)
    {
    case 'h': // print help message
      usage();
      break;
    case 'v': // emit additional diagnostic info
      verbose = 1;
      break;
    case 'p':          // don't print a prompt
      emit_prompt = 0; // handy for automatic testing
      break;
    default:
      usage();
    }
  }

  //
  // Install the signal handlers
  //

  //
  // These are the ones you will need to implement
  //
  Signal(SIGINT, sigint_handler);   // ctrl-c
  Signal(SIGTSTP, sigtstp_handler); // ctrl-z
  Signal(SIGCHLD, sigchld_handler); // Terminated or stopped child

  //
  // This one provides a clean way to kill the shell
  //
  Signal(SIGQUIT, sigquit_handler);

  //
  // Initialize the job list
  //
  initjobs(jobs);

  //
  // Execute the shell's read/eval loop
  //
  for (;;)
  {
    //
    // Read command line
    //
    if (emit_prompt)
    {
      printf("%s", prompt);
      fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
    {
      app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin))
    {
      fflush(stdout);
      exit(0);
    }

    //
    // Evaluate command line
    //
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  }

  exit(0); //control never reaches here
}

/////////////////////////////////////////////////////////////////////////////
//
// eval - Evaluate the command line that the user has just typed in
//
// If the user has requested a built-in command (quit, jobs, bg or fg)
// then execute it immediately. Otherwise, fork a child process and
// run the job in the context of the child. If the job is running in
// the foreground, wait for it to terminate and then return.  Note:
// each child process must have a unique process group ID so that our
// background children don't receive SIGINT (SIGTSTP) from the kernel
// when we type ctrl-c (ctrl-z) at the keyboard.
//
void eval(char *cmdline)
{
  char *argv[MAXARGS];               /* Argument list execve() */
  int bg = parseline(cmdline, argv); /* Should the job run in bg or fg? */
  pid_t pid;                         /* Process id */
  sigset_t mask; //Collection of values for the signals

  //First, check if our command is a built in command by running bultin_cmd,
  //if it is a builtin cmd, it will be run in the builtin_cmd function
  if (!builtin_cmd(argv))
  {
    sigemptyset(&mask); //Initialize our signal mask as empty (all signals will be received).
    sigaddset(&mask, SIGCHLD); //Add signal to signal collection
    sigprocmask(SIG_BLOCK, &mask, NULL); //block signals to help with race condition
    
    //Fork to create a bg process
    pid = fork();

    //if child process
    if (pid == 0)
    {
      //Set new signal to mask and return NULL (no old mask)
      sigprocmask(SIG_UNBLOCK, &mask, NULL);
      //Create a new process group
      setpgid(0, 0);

      //Runs the user's code, returns -1 if error, so if we get a -1, we have an invalid command
      if (execv(argv[0], argv) == -1)
      {
        //if the command is not found, output error
        printf("%s: Command not found\n", argv[0]);
        //and quit out of the background process
        exit(0);
      }
    }
    else
    {
      //if we get here, we're the parent process

      //If we are the foreground process
      if (!bg)
      {
        //add the job to the fg
        addjob(jobs, pid, FG, cmdline);
        //Set new signal to mask and return NULL (no old mask)
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        //waits for the current fg process to end.
        waitfg(pid);
      }
      else
      {
        //We're a background process
        //add the job to the bg
        addjob(jobs, pid, BG, cmdline);
        //Set new signal to mask and return NULL (no old mask)
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        //Prints the jobid, process id, and command
        printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
      }
    }
  }
  //Ignore empty command line arguments
  if (argv[0] == NULL)
    return;

  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv)
{
  //if user wants to quit, quit.
  if (!strcmp(argv[0], "quit"))
  {
    //well, quit
    exit(0);
  }
  //if user enters "bg" or "fg", run the following command
  else if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg"))
  {
    //run the do_bgfg function
    do_bgfg(argv);
    //return that it was a built in command
    return 1;
  }
  //if user enters jobs, list the jobs
  else if (!strcmp(argv[0], "jobs"))
  {
    //well, 
    listjobs(jobs);
    //return that it was a built in command
    return 1;
    
  }
  else if (!strcmp(argv[0], "&"))
  {
    //if user enters &, ignore it
    //return that it was a built in command
    return 1;
  }
  //otherwise, it's not a builtin command
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv)
{
  struct job_t *jobp = NULL;

  //Ignore command if no argument
  if (argv[1] == NULL)
  {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }

  //Parse the required PID or %JID arg
  if (isdigit(argv[1][0]))
  {
    pid_t pid = atoi(argv[1]);
    if (!(jobp = getjobpid(jobs, pid)))
    {
      printf("(%d): No such process\n", pid);
      return;
    }
  }
  else if (argv[1][0] == '%')
  {
    int jid = atoi(&argv[1][1]);
    if (!(jobp = getjobjid(jobs, jid)))
    {
      printf("%s: No such job\n", argv[1]);
      return;
    }
  }
  else
  {
    printf("%s: argument must be a PID or %%jobid\n", argv[0]);
    return;
  }

  //
  // You need to complete rest. At this point,
  // the variable 'jobp' is the job pointer
  // for the job ID specified as an argument.
  //
  // Your actions will depend on the specified command
  // so we've converted argv[0] to a string (cmd) for
  // your benefit.
  //
  string cmd(argv[0]);

  //if "bg", move to background
  if (!strcmp(argv[0], "bg"))
  {
    //change state to bg
    jobp->state = BG;
    //kills the job to start again
    kill(-jobp->pid, SIGCONT);
    //printing the job id, process id, and command
    printf("[%d] (%d) %s", jobp->jid, jobp->pid, jobp->cmdline);
  }
  if (!strcmp(argv[0], "fg"))
  {
    //change state to fg
    jobp->state = FG;
    //kills the job to start again
    kill(-jobp->pid, SIGCONT);
    //wait since we can only have a single fg process running at a time
    waitfg(jobp->pid);
  }

  return;
}

// waitfg - Block until process pid is no longer the foreground process
void waitfg(pid_t pid)
{
  //get job from pid
  job_t *job = getjobpid(jobs, pid);
  //if the job is still in the foreground, wait
  while (job->state == FG)
  {
    // waits for current process to end
    pause();
  }
  return;
}

// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.

void sigchld_handler(int sig)
{
  //initialize variables
  pid_t pid;
  int status;

  //waits for zombie children
  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
  {
    //if process was stopped
    if (WIFSTOPPED(status))
    {
      job_t *job = getjobpid(jobs, pid);
      job->state = ST;
      fprintf(stdout, "Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
    }
    else if (WIFSIGNALED(status))
    {
      fprintf(stdout, "Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
      deletejob(jobs, pid);
    }
    else if (WIFEXITED(status))
    {
      deletejob(jobs, pid);
    }
  }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.
//
void sigint_handler(int sig)
{
  pid_t pid = fgpid(jobs);

  if (pid != 0)
  {
    //if valid pid, kill
    kill(-pid, sig);
  }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.
//
void sigtstp_handler(int sig)
{
  pid_t pid = fgpid(jobs);
  if (pid != 0)
  {
    //if valid pid, pause
    kill(-pid, sig);
  }
  return;
}

/*********************
 * End signal handlers
 *********************/
