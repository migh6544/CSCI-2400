// 
// tsh - A tiny shell program with job control
// 
// Michael Ghattas / Migh6544
// Note: Some parts of my solution are inspired by information gathered from stackoverflow.com, geeksforgeeks.com and GitHub.com.
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
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
    case 'h':             // print help message
      usage();
      break;
    case 'v':             // emit additional diagnostic info
      verbose = 1;
      break;
    case 'p':             // don't print a prompt
      emit_prompt = 0;  // handy for automatic testing
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
  Signal(SIGINT,  sigint_handler);   // ctrl-c
  Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
  Signal(SIGCHLD, sigchld_handler);  // Terminated or stopped child

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
  for(;;) {
    //
    // Read command line
    //
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin)) {
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

// Masks for signal blocking
sigset_t mask, prevmask;
void eval(char *cmdline)
{
    /* Parse command line */
    //
    // The 'argv' vector is filled in by the parseline
    // routine below. It provides the arguments needed
    // for the execve() routine, which you'll need to
    // use below to launch a process.
    //
    char *argv[MAXARGS];
    // This is an array that holds the values from the parsed input
    char buf[MAXLINE];
    // This save the process ID so we can use it if needed (The current process id from fork)
    pid_t pid;
    // This creates a mask to supress the childrens output
    sigset_t mask; 
    // THis initializes the empty signal set
    if(sigemptyset(&mask))
    {
        unix_error("eval: sigemptyset error");  
    }
    // This adds SIGCHLD signal to the signal set
    if(sigaddset(&mask, SIGCHLD))
    {
        unix_error("eval: sigaddset error");
    }
    // This copies the command line into 'buf'
    strcpy(buf, cmdline);
    
    /* If the job should run, then the 'bg' variable is TRUE. If it should run in 'fg', then FALSE. */
    
    // Enviroment parsing needed for execve
    int bg = parseline(cmdline, argv); 
    if (argv[0] == NULL)
    {
        // Ignoring empty lines
        return; 
    }
/* 
    - If it is not a builtin_cmd, we need to fork and exec a child process. 
    - If it is a builtin_cmd we will process it with our builtin_cmd function.
    - sigprocmask(SIG_BLOCK, &masker, NULL): Blocks SIGCHLD singals before forking and will be unblocked after addjob is called.
    - sigprocmask(SIG_UNBLOCK, &masker, NULL): Unblocks the signals we blocked before add jobs.
*/
    // If not a built in command, then fork()
    if (!builtin_cmd(argv))
    {
        // This makes sure the parent blocks SIGCHLD signal temporarily as needed
        sigprocmask(SIG_BLOCK, &mask, 0);
        // Fork rerutns pid of child process to parrent and 0 to child. Child is what runs the user job
        if( (pid = fork()) <= 0)
        { 
            // This Create a child and its run process within the if-statement (ID has to be greater than 0 and -1 = error)
            if (pid < 0) unix_error("fork: forking error");
                // This sets group ID for all of the children (Sets group id for all fg processes so bg processes don't associate with fg processes)
                setpgid(0,0); 
                // Running child Process
                sigprocmask(SIG_UNBLOCK, &mask, 0);
            // This fork creates a child process where execv replaces the current execution with a newly loaded program image (PID unchanged)
            if(execve(argv[0], argv, environ) < 0)
            {
                // The first element must be > 0. If the command entered is not found when we exec it, a negative value is returned if the command is not found (If so, exit so we don't end up running multiple instances of our shell)
                printf("%s: Command not found. \n", argv[0]);
                // This will never return unless the command is a unrecognized command
                exit(0);
            }
        }
        // This will execute if we are in the an FG process (bg is false)
        if(!bg)
        { 
            int status;
            // If not bg, then add job as fg (add job to struct with fg state)
            addjob(jobs, pid, FG, cmdline);
            // Parent unblocks SIGCHLD 
            sigprocmask(SIG_UNBLOCK, &mask, 0);
            // Parent waits for foreground job to terminate (Wait for the child process and reap it, avoid zombies and /or defunct processes)
            waitfg(pid);
        }
        else
        {
            // If if its bg, add job as bg (add job to struct with bg state)
            addjob(jobs, pid, BG, cmdline);
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
            // Parent then unblocks SIGCHLD
            sigprocmask(SIG_UNBLOCK, &mask, 0);
        }
    }
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
/* 
    - If builtin_cmd returns 0, the shell creates a child process and executres the requested program inside the child. 
    - If the user has reuqested for the program to run in the background, then the shell returns to the top of the loop and waits for the next command line.
*/
    // This is the same command as that used in do_bgfg, will operate in a way so not to have to use strcmpr for every if statement
    string cmd(argv[0]);
    // General quit command (Helps kill all jobs before exiting to prevent zombies)
    if(cmd == "quit")
    {
        exit(0);
    }
    // Ignore command for background process that have no arguement
    if(cmd == "&")
    { 
        return 1;
    }
    // Print jobs list
    if(cmd == "jobs")
    {
        listjobs(jobs);
        return 1;
    }
    // Switch process between bg & fg
    if(cmd == "bg" || cmd == "fg")
    {
        do_bgfg(argv);
        return 1; 
    }
    /* not a builtin command */
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv) 
{  
    struct job_t *jobp=NULL;

    /* Ignore command if no argument */
    if (argv[1] == NULL)
    {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    /* Parse the required PID or %JID arg */
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
    if (cmd == "bg")
    {
        if(kill(-(jobp->pid), SIGCONT) < 0) unix_error("do_bgfg: Kill error");
            // Change the job state to bg
            jobp->state = BG;
            // Print info and don't wait because job is in bg.
            printf("[%d] (%d) %s", (jobp->jid), (jobp->pid), (jobp->cmdline));
    }
    else if (cmd == "fg")
    {
        if(kill(-(jobp->pid), SIGCONT) < 0) unix_error("do_bgfg: Kill error");
            // Change the job state to fg
            jobp->state = FG;
            // Wait for the job because it's now in fg.
            waitfg(jobp->pid); 
    }
    else
    {
        unix_error("do_bgfg error");
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer the foreground process
//
void waitfg(pid_t pid)
{
    // Endless loop till fg job is done
    while(pid == fgpid(jobs))
    {
        // While the pid = fg pid, do nothing. If it is not a fg pid, stop sleeping.
        sleep(0);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Signal handlers
//


/////////////////////////////////////////////////////////////////////////////
//
// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.  
//

/*
While there are children still out there to kill (Zombies, oprhens etc.):
    - WNOHANG = Checks child processes without causing the caller to be suspended (If waitpid returns promptly instead of waiting and/or if there is no child ready. If there is at least one child process thats its status information is not available, waitpid will return 0)
    - WUNTRACED = Status of stopped and/or terminated children
    - WNOHANG | WUNTRACED = To return immediately, with a return value = 0, should none of the children in the wait-set have stopped and/or terminated, or if a return value is equal to the PID of one of the stopped and/or terminated children.
    - WTERMSIG determines which signal causes child to exit. Accourdingly, this exits because it raised a signal that caused it to exit
    - WSTOPSIG tells which signal causes child to stop. Accourdingly, this handles if the child process was stopped by the delivery of a signal and/or is already currently stopped   
*/
void sigchld_handler(int sig) 
{
    // The location where waitpid can store a status value. If value is zero, child process returns zero (Otherwise it will be analyzed)
    int status;
    //This specifies the child processes the caller would want to wait for. If pid value < 0, waitpid() waits for the child process to end. (leting the child processes end before reaping them)
    pid_t pid;
    // Return a value > 0 needed from waitpid function before we can delete and/or reap a child.
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0 )
    {
        // Check if child has been terminated normally
        if (WIFEXITED(status))
        {   
            deletejob(jobs, pid);
        }
        // Check if child was terminated by an uncaught signal
        if (WIFSIGNALED(status))
        {  
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
            deletejob(jobs,pid);
        }
        // Check if the returned child process is currently stopped
        if (WIFSTOPPED(status))
        {     
            getjobpid(jobs, pid)->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status) );
        }
    }
    if (pid < 0 && errno != ECHILD)
    {
        printf("waitpid error: %s\n", strerror(errno));
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
    // First get the fg Process ID
    pid_t pid = fgpid(jobs);
    // Return must be a value greater than 0 to be a valid fg job (0 = no fg process found)
    if (pid != 0)
    {
        //If foreground process exists, negative pid kills all processes in group (Default action for sigint)
        if(kill(-(pid), SIGINT) < 0 )
        {
            unix_error("kill: termination error"); 
        } 

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
    // First get the fg Process ID
    pid_t pid = fgpid(jobs);
    if(pid != 0) 
    {
        // Call kill(-fgpid, sig);
        if(kill(-pid, SIGTSTP) < 0)
        {
            unix_error("kill: stop error");
        }
    }
    return;
}

/*********************
 * End signal handlers
 *********************/