# RTEST01 
./sdriver.pl -t trace01.txt -s ./tshref -a "-p"
#
# trace01.txt - Properly terminate on EOF.
#

# RTEST02
./sdriver.pl -t trace02.txt -s ./tshref -a "-p"
#
# trace02.txt - Process builtin quit command.
#

# RTEST 03
./sdriver.pl -t trace03.txt -s ./tshref -a "-p"
#
# trace03.txt - Run a foreground job.
#
tsh> quit

# RTEST 04
./sdriver.pl -t trace04.txt -s ./tshref -a "-p"
#
# trace04.txt - Run a background job.
#
tsh> ./myspin 1 &
[1] (435) ./myspin 1 &

# RTEST 05
./sdriver.pl -t trace05.txt -s ./tshref -a "-p"
#
# trace05.txt - Process jobs builtin command.
#
tsh> ./myspin 2 &
[1] (441) ./myspin 2 &
tsh> ./myspin 3 &
[2] (443) ./myspin 3 &
tsh> jobs
[1] (441) Running ./myspin 2 &
[2] (443) Running ./myspin 3 &

# RTEST 06
./sdriver.pl -t trace06.txt -s ./tshref -a "-p"
#
# trace06.txt - Forward SIGINT to foreground job.
#
tsh> ./myspin 4
Job [1] (450) terminated by signal 2

# RTEST 07
./sdriver.pl -t trace07.txt -s ./tshref -a "-p"
#
# trace07.txt - Forward SIGINT only to foreground job.
#
tsh> ./myspin 4 &
[1] (456) ./myspin 4 &
tsh> ./myspin 5
Job [2] (458) terminated by signal 2
tsh> jobs
[1] (456) Running ./myspin 4 &

# RTEST 08
./sdriver.pl -t trace08.txt -s ./tshref -a "-p"
#
# trace08.txt - Forward SIGTSTP only to foreground job.
#
tsh> ./myspin 4 &
[1] (465) ./myspin 4 &
tsh> ./myspin 5
Job [2] (467) stopped by signal 20
tsh> jobs
[1] (465) Running ./myspin 4 &
[2] (467) Stopped ./myspin 5 

# RTEST 09 
./sdriver.pl -t trace09.txt -s ./tshref -a "-p"
#
# trace09.txt - Process bg builtin command
#
tsh> ./myspin 4 &
[1] (475) ./myspin 4 &
tsh> ./myspin 5
Job [2] (477) stopped by signal 20
tsh> jobs
[1] (475) Running ./myspin 4 &
[2] (477) Stopped ./myspin 5 
tsh> bg %2
[2] (477) ./myspin 5 
tsh> jobs
[1] (475) Running ./myspin 4 &
[2] (477) Running ./myspin 5 

# RTEST 10
./sdriver.pl -t trace10.txt -s ./tshref -a "-p"
#
# trace10.txt - Process fg builtin command. 
#
tsh> ./myspin 4 &
[1] (486) ./myspin 4 &
tsh> fg %1
Job [1] (486) stopped by signal 20
tsh> jobs
[1] (486) Stopped ./myspin 4 &
tsh> fg %1
tsh> jobs

# RTEST 11
./sdriver.pl -t trace11.txt -s ./tshref -a "-p"
#
# trace11.txt - Forward SIGINT to every process in foreground process group
#
tsh> ./mysplit 4
Job [1] (496) terminated by signal 2
tsh> /bin/ps a
    PID TTY      STAT   TIME COMMAND
     70 pts/0    Ss+    0:00 /bin/bash -l
     84 pts/1    Ss     0:00 /bin/bash -l
    491 pts/1    S+     0:00 make rtest11
    492 pts/1    S+     0:00 /bin/sh -c ./sdriver.pl -t trace11.txt -s ./tshref -a "-p"
    493 pts/1    S+     0:00 /usr/bin/perl ./sdriver.pl -t trace11.txt -s ./tshref -a -p
    494 pts/1    S+     0:00 ./tshref -p
    499 pts/1    R      0:00 /bin/ps a

# RTEST 12 
./sdriver.pl -t trace12.txt -s ./tshref -a "-p"
#
# trace12.txt - Forward SIGTSTP to every process in foreground process group
#
tsh> ./mysplit 4
Job [1] (505) stopped by signal 20
tsh> jobs
[1] (505) Stopped ./mysplit 4 
tsh> /bin/ps a
    PID TTY      STAT   TIME COMMAND
     70 pts/0    Ss+    0:00 /bin/bash -l
     84 pts/1    Ss     0:00 /bin/bash -l
    500 pts/1    S+     0:00 make rtest12
    501 pts/1    S+     0:00 /bin/sh -c ./sdriver.pl -t trace12.txt -s ./tshref -a "-p"
    502 pts/1    S+     0:00 /usr/bin/perl ./sdriver.pl -t trace12.txt -s ./tshref -a -p
    503 pts/1    S+     0:00 ./tshref -p
    505 pts/1    T      0:00 ./mysplit 4
    506 pts/1    T      0:00 ./mysplit 4
    509 pts/1    R      0:00 /bin/ps a

# RTEST 13
./sdriver.pl -t trace13.txt -s ./tshref -a "-p"
#
# trace13.txt - Restart every stopped process in process group
#
tsh> ./mysplit 4
Job [1] (515) stopped by signal 20
tsh> jobs
[1] (515) Stopped ./mysplit 4 
tsh> /bin/ps a
    PID TTY      STAT   TIME COMMAND
     70 pts/0    Ss+    0:00 /bin/bash -l
     84 pts/1    Ss     0:00 /bin/bash -l
    510 pts/1    S+     0:00 make rtest13
    511 pts/1    S+     0:00 /bin/sh -c ./sdriver.pl -t trace13.txt -s ./tshref -a "-p"
    512 pts/1    S+     0:00 /usr/bin/perl ./sdriver.pl -t trace13.txt -s ./tshref -a -p
    513 pts/1    S+     0:00 ./tshref -p
    515 pts/1    T      0:00 ./mysplit 4
    516 pts/1    T      0:00 ./mysplit 4
    519 pts/1    R      0:00 /bin/ps a
tsh> fg %1
tsh> /bin/ps a
    PID TTY      STAT   TIME COMMAND
     70 pts/0    Ss+    0:00 /bin/bash -l
     84 pts/1    Ss     0:00 /bin/bash -l
    510 pts/1    S+     0:00 make rtest13
    511 pts/1    S+     0:00 /bin/sh -c ./sdriver.pl -t trace13.txt -s ./tshref -a "-p"
    512 pts/1    S+     0:00 /usr/bin/perl ./sdriver.pl -t trace13.txt -s ./tshref -a -p
    513 pts/1    S+     0:00 ./tshref -p
    522 pts/1    R      0:00 /bin/ps a

# RTEST 14 
./sdriver.pl -t trace14.txt -s ./tshref -a "-p"
#
# trace14.txt - Simple error handling
#
tsh> ./bogus
./bogus: Command not found
tsh> ./myspin 4 &
[1] (530) ./myspin 4 &
tsh> fg
fg command requires PID or %jobid argument
tsh> bg
bg command requires PID or %jobid argument
tsh> fg a
fg: argument must be a PID or %jobid
tsh> bg a
bg: argument must be a PID or %jobid
tsh> fg 9999999
(9999999): No such process
tsh> bg 9999999
(9999999): No such process
tsh> fg %2
%2: No such job
tsh> fg %1
Job [1] (530) stopped by signal 20
tsh> bg %2
%2: No such job
tsh> bg %1
[1] (530) ./myspin 4 &
tsh> jobs
[1] (530) Running ./myspin 4 &

# RTEST 15
./sdriver.pl -t trace15.txt -s ./tshref -a "-p"
#
# trace15.txt - Putting it all together
#
tsh> ./bogus
./bogus: Command not found
tsh> ./myspin 10
Job [1] (549) terminated by signal 2
tsh> ./myspin 3 &
[1] (551) ./myspin 3 &
tsh> ./myspin 4 &
[2] (553) ./myspin 4 &
tsh> jobs
[1] (551) Running ./myspin 3 &
[2] (553) Running ./myspin 4 &
tsh> fg %1
Job [1] (551) stopped by signal 20
tsh> jobs
[1] (551) Stopped ./myspin 3 &
[2] (553) Running ./myspin 4 &
tsh> bg %3
%3: No such job
tsh> bg %1
[1] (551) ./myspin 3 &
tsh> jobs
[1] (551) Running ./myspin 3 &
[2] (553) Running ./myspin 4 &
tsh> fg %1
tsh> quit

# RTEST 16
./sdriver.pl -t trace16.txt -s ./tshref -a "-p"
#
# trace16.txt - Tests whether the shell can handle SIGTSTP and SIGINT
#     signals that come from other processes instead of the terminal.
#
tsh> ./mystop 2
Job [1] (567) stopped by signal 20
tsh> jobs
[1] (567) Stopped ./mystop 2
tsh> ./myint 2
Job [2] (570) terminated by signal 2