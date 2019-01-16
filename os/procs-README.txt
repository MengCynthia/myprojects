Documentation for Kernel Assignment 1
=====================================

+------------------------+
| BUILD & RUN (Required) |
+------------------------+

Replace "(Comments?)" with the command the grader should use to compile your kernel (should simply be "make").
    To compile the kernel, the grader should type:
	 make clean; make
If you have additional instruction for the grader, replace "(Comments?)" with your instruction (or with the word "none" if you don't have additional instructions):
    Additional instructions for building/running this assignment: none

+-------------------------+
| SELF-GRADING (Required) |
+-------------------------+

Replace each "(Comments?)" with appropriate information and each stand-alone "?"
with a numeric value:

(A.1) In main/kmain.c:
    (a) In bootstrap(): 3 out of 3 pts
    (b) In initproc_create(): 3 out of 3 pts

(A.2) In proc/proc.c:
    (a) In proc_create(): 4 out of 4 pts
    (b) In proc_cleanup(): 7 out of 7 pts
    (c) In do_waitpid(): 6 out of 6 pts

(A.3) In proc/kthread.c:
    (a) In kthread_create(): 2 out of 2 pts
    (b) In kthread_cancel(): 1 out of 1 pt
    (c) In kthread_exit(): 3 out of 3 pts

(A.4) In proc/sched.c:
    (a) In sched_wakeup_on(): 1 out of 1 pt

(A.5) In proc/sched.c:
    (a) In sched_make_runnable(): 1 out of 1 pt

(A.6) In proc/kmutex.c:
    (a) In kmutex_lock(): 1 out of 1 pt
    (b) In kmutex_lock_cancellable(): 1 out of 1 pt
    (c) In kmutex_unlock(): 2 out of 2 pts

(B) Kshell : 20 out of 20 pts
    What is your kshell command to invoke faber_thread_test(): faber
    What is your kshell command to invoke sunghan_test(): sunghan
    What is your kshell command to invoke sunghan_deadlock_test(): deadlock

(C.1) waitpid any test, etc. (4 out of 4 pts)
(C.2) Context switch test (1 out of 1 pt)
(C.3) wake me test, etc. (2 out of 2 pts)
(C.4) wake me uncancellable test, etc. (2 out of 2 pts)
(C.5) cancel me test, etc. (4 out of 4 pts)
(C.6) reparenting test, etc. (2 out of 2 pts)
(C.7) show race test, etc. (3 out of 3 pts)
(C.8) kill child procs test (2 out of 2 pts)
(C.9) proc kill all test (2 out of 2 pts)

(D.1) sunghan_test(): producer/consumer test (9 out of 9 pts)
(D.2) sunghan_deadlock_test(): deadlock test (4 out of 4 pts)

(E) Additional self-checks: (10 out of 10 pts)
    Please provide details, add subsections and/or items as needed; or, say that "none is needed".
    Details: 
We create additional test to check when one thread holds mutex lock (using kmutex_lock_cancellable), and if it is cancelled, it should give up the lock

We create a seperate process to create two threads run this test. The parent process will wait for two child process to terminate

The two threads will do the following:
	Thread1 locks mutex
	make sched_switch (now switch to thread 2)
 	when sched_switch returns, it unlocks mutex (now thread 2 owns the lock)
	cancel thread 2
	do_exit (give cpu to next thread in the run queue which is thread 2)

	Thread2 calls cancellable lock mutex - it is blocked and sleep onto mutex queue (give up cpu back to thread 1)
   Once thread 2 wakes up, it checks if it is cancelled, if so, it will give up mutex and return -EINR
	Thread2 then calls do_exit

To run this test: Enter cmutex in kshell

Please note:
According to grading guideline, we use DBG_TEMP to print test messages (start testing and end testing messages). Please use "DBG = error,temp,test" in Config.mk to compile and run this test


Missing/incomplete required section(s) in README file (procs-README.txt): (0 pts)
Submitted binary file : (0 pts)
Submitted extra (unmodified) file : (0 pts)
Wrong file location in submission : (0 pts)
Extra printout when running with DBG=error,test in Config.mk : (0 pts)
Incorrectly formatted or mis-labeled "conforming dbg() calls" : (0 pts)
Cannot compile : (0 pts)
Compiler warnings : (0 pts)
"make clean" : (0 pts)
Kernel panic : (0 pts)
Kernel freezes : (0 pts)
Cannot halt kernel cleanly : (0 pts)

+--------------------------------------+
| CONTRIBUTION FROM MEMBERS (Required) |
+--------------------------------------+

1)  Names and USC e-mail addresses of team members: 
Dennis Wang - wang279@usc.edu
Meng Shen - mengs@usc.edu
Yu-Chiang Liao - yuchianl@usc.edu
Peng Jiang - jiangp@usc.edu

2)  Is the following statement correct about your submission (please replace
        "(Comments?)" with either "yes" or "no", and if the answer is "no",
        please list percentages for each team member)?  "Each team member
        contributed equally in this assignment": Yes

+---------------------------------+
| BUGS / TESTS TO SKIP (Required) |
+---------------------------------+

Are there are any tests mentioned in the grading guidelines test suite that you
know that it's not working and you don't want the grader to run it at all so you
won't get extra deductions, please replace "(Comments?)" below with your list.
(Of course, if the grader won't run such tests in the plus points section, you
will not get plus points for them; if the garder won't run such tests in the
minus points section, you will lose all the points there.)  If there's nothing
the grader should skip, please replace "(Comments?)" with "none".

Please skip the following tests: none

+-----------------------------------------------+
| OTHER (Optional) - Not considered for grading |
+-----------------------------------------------+

Comments on design decisions: 
We use busy wait to implement sched_switch, so when testing dbg conforming calls, it will keep printing which can slow down faber thread test. 
However, if DBG = error,test is used, faber thread test can run expectedly

