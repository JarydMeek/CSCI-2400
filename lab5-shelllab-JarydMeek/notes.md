# Fork / Execve

## Entry/Exit logic of fork
When fork() is called, it creates a new process. This new process is the same as the previous process, so there is basically two instances of the program running. 

**Return values ->**

Child process returns pid == 0

Parent process returns pid == child

if -1 is returned there is an error and no child is created


**Exit()**

When exit(0) is called, it exits the current process with a success. Any open file descriptors belonging to the process are closed and any children of the process are inherited by process 1, init, and the process parent is sent a SIGCHLD signal.

exit(1), it exits the current process with a failure

## Exec args and handling failure
First, we check if the args are a built-in command, if it is, we run the command directly, if it isn't we use execv to run the command for us.

execv(path,argv) causes the current process to abandon the program that it is running and start running the program in file path.

execv will return -1 if there is an error. (basically it's an unknown command)


## Process group purpose and implementations
A process group is a collection of processes. It is used to control the distribution of a signal: when a signal is directed to a process group, the signal is delivered to each process that is a member of the group. 

We put all of our processes into one process group to allow us to signal them all at once.

If pgid=0, setpgid uses pgid=pid of the calling process
(line 184)

# Race Condition
There is a 'race' between the events and if the wrong events win, the program fails.

The collection of signals that are currently blocked is called the signal mask. Each process has its own signal mask. You can block or unblock signals with total flexibility by modifying the signal mask.

## Jobs modification
The parent always handles adding the job and the child always sends the signal that ends up killing the job. ( rip job 2020-2020 ): )

## Race and fix for race
The race condition is that the child could complete the command before the parent has time to run addjob, so the job will be deleted before it is added and the job will never be deleted.

That's bad. ):

We solved this by blocking the signals until after we add the job (in the parent), and before we run execv in the child, to make sure our race completes in the right order.

AKA we're rigging the race 4head.

# SIGTSTP

## Code Function
sigchild checks to make sure the process is still running, if it is, it sends the kill command with the process and the sig passed to the function, in sigtstp, this would suspend the process 

## Control flow when Ctrl - Z
Control-Z suspends the most recent foreground process (the last process to interact with the tty) (unless that process takes steps to ignore suspension, like shells normally do). This will generally bring you back to your shell, from which you can generally enter the command bg to move the just-suspended process to the background (letting it continue to run) or fg to bring it back to the foreground.

# SIGCHILD

## Three cases of SIGCHLD (stopped, normal, signaled)
**Stopped -**
if the process was stopped, we get the job pid associated with that process, and set it's state to be stopped. We then print a nice messages saying that it's stopped

**Normal -**
if the process executes normally, we just delete the job with deletejob to kill the zombie children🧟‍♂️

**Signaled -**
if the process was terminated, we get the job pid associated with that process, and we print out a message saying the job was stopped, as well as deleting the job with the deletejob command

## State of child is updated for Ctrl - C
We delete the job since we are terminating it.

## State of child is updated for Ctrl - Z
We set the job state to "ST" to signify that the job is stopped
