Documentation for Kernel Assignment 2
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

(A.1) In fs/vnode.c:
    (a) In special_file_read(): 6 out of 6 pts
    (b) In special_file_write(): 6 out of 6 pts

(A.2) In fs/namev.c:
    (a) In lookup(): 6 out of 6 pts
    (b) In dir_namev(): 10 out of 10 pts
    (c) In open_namev(): 2 out of 2 pts

(A.3) In fs/vfs_syscall.c:
    (a) In do_write(): 4 out of 4 pts
    (b) In do_mknod(): 2 out of 2 pts
    (c) In do_mkdir(): 2 out of 2 pts
    (d) In do_rmdir(): 2 out of 2 pts
    (e) In do_unlink(): 2 out of 2 pts
    (f) In do_stat(): 2 out of 2 pts

(B) vfstest: 41 out of 41 pts
    What is your kshell command to invoke vfstest_main(): vfstest_main

(C.1) faber_fs_thread_test (3 out of 3 pts)
    What is your kshell command to invoke faber_fs_thread_test(): "thrtest n" (without quotes, where n can be 2, 3, or 4)
(C.2) faber_directory_test (2 out of 2 pts)
    What is your kshell command to invoke faber_directory_test(): "dirtest n" (without quotes, where n can be 10, 20, 30, or 40)

(D) Self-checks: (10 out of 10 pts)
    Please provide details, add subsections and/or items as needed; or, say that "none is needed".
    Details: since vfstest only test unsuccessful rename operation, we create a test to do the following
	 1. create a file by calling do_open 
	 2. rename it to a different file 
	 3. call do_close to close opened file
	 4. delete the renamed file
	 5. print success message if everything is ok

To run this test: Enter vfs_rename in kshell

Please note:
According to grading guideline, we use DBG_TEMP to print test messages. Please use "DBG = error,temp,test" in Config.mk to compile and run this test

Missing/incomplete required section(s) in README file (vfs-README.txt): (0 pts)
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
        contributed equally in this assignment": yes

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

Comments on design decisions: none

