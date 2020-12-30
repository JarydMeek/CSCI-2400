//
// tsh - A tiny shell program with job control
//
// John Korhel joko3539
// Jaryd Meek jame7931
//

using namespace std;

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
    char *argv[MAXARGS];
    //int to record for bg   
    int bg;         
    pid_t pid;      
    sigset_t mask;
    
    // parse the line
    bg = parseline(cmdline, argv);
    //check if valid builtin_cmd
    if(!builtin_cmd(argv)) { 
        
        // blocking first
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);
        // forking
        if((pid = fork()) < 0){
            unix_error("forking error");
        }
        // child
        else if(pid == 0) {
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            setpgid(0, 0);
            //check if command is there
            if(execvp(argv[0], argv) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(1);
            }
        } 
        // parent add job first
        else {
            if(!bg){
                addjob(jobs, pid, FG, cmdline);
            }
            else {
                addjob(jobs, pid, BG, cmdline);
            }
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            
            //if bg/fg
            if (!bg){
                //wait for fg
                waitfg(pid);
            } 
            else {
                //print for bg
                printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
            }
        }
    }
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
    buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
    }
    else {
    delim = strchr(buf, ' ');
    }

    while (delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces */
           buf++;

    if (*buf == '\'') {
        buf++;
        delim = strchr(buf, '\'');
    }
    else {
        delim = strchr(buf, ' ');
    }
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
    return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
    argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }
    else if (!strcmp("&", argv[0])){
        return 1;
    }
    else if (!strcmp("jobs", argv[0])) {  
        listjobs(jobs);  
        return 1;  
    }  
    else if (!strcmp("bg", argv[0]) || !(strcmp("fg", argv[0]))) {  
        //call bgfg
        do_bgfg(argv);  
        return 1;  
    }  
    return 0;
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{   
    struct job_t *job;
    char *tmp;
    int jid;
    pid_t pid;

    tmp = argv[1];
    
    // if id does not exist
    if(tmp == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    
    // if it is a jid
    if(tmp[0] == '%') {  
        jid = atoi(&tmp[1]); 
        //get job
        job = getjobjid(jobs, jid);
        if(job == NULL){  
            printf("%s: No such job\n", tmp);  
            return;  
        }else{
            //get the pid if a valid job for later to kill
            pid = job->pid;
        }
    } 
    // if it is a pid
    else if(isdigit(tmp[0])) { 
        //get pid
        pid = atoi(tmp); 
        //get job 
        job = getjobpid(jobs, pid); 
        if(job == NULL){  
            printf("(%d): No such process\n", pid);  
            return;  
        }  
    }  
    else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    //kill for each time
    kill(-pid, SIGCONT);
    
    if(!strcmp("fg", argv[0])) {
        //wait for fg
        job->state = FG;
        waitfg(job->pid);
    } 
    else{
        //print for bg
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
        job->state = BG;
    } 
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    struct job_t* job;
    job = getjobpid(jobs,pid);
    //check if pid is valid
    if(pid == 0){
        return;
    }
    if(job != NULL){
        //sleep
        while(pid==fgpid(jobs)){
        }
    }
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int status;  
    pid_t pid;  
    
    while ((pid = waitpid(fgpid(jobs), &status, WNOHANG|WUNTRACED)) > 0) {  
        if (WIFSTOPPED(status)){ 
            //change state if stopped
            getjobpid(jobs, pid)->state = ST;
            int jid = pid2jid(pid);
            printf("Job [%d] (%d) Stopped by signal %d\n", jid, pid, WSTOPSIG(status));
        }  
        else if (WIFSIGNALED(status)){
            //delete is signaled
            int jid = pid2jid(pid);  
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }  
        else if (WIFEXITED(status)){  
            //exited
            deletejob(jobs, pid);  
        }  
    }  
    return; 
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    pid_t pid = fgpid(jobs);  
    
    //check for valid pid
    if (pid != 0) {     
        kill(-pid, sig);
    }   
    return;   
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    pid_t pid = fgpid(jobs);  
    //check for valid pid
    if (pid != 0) { 
        kill(-pid, sig);  
    }  
    return;   
}