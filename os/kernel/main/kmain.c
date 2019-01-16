/******************************************************************************/
/* Important Fall 2018 CSCI 402 usage information:                            */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#include "types.h"
#include "globals.h"
#include "kernel.h"
#include "errno.h"

#include "util/gdb.h"
#include "util/init.h"
#include "util/debug.h"
#include "util/string.h"
#include "util/printf.h"

#include "mm/mm.h"
#include "mm/page.h"
#include "mm/pagetable.h"
#include "mm/pframe.h"

#include "vm/vmmap.h"
#include "vm/shadowd.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "main/acpi.h"
#include "main/apic.h"
#include "main/interrupt.h"
#include "main/gdt.h"

#include "proc/sched.h"
#include "proc/proc.h"
#include "proc/kthread.h"

#include "drivers/dev.h"
#include "drivers/blockdev.h"
#include "drivers/disk/ata.h"
#include "drivers/tty/virtterm.h"
#include "drivers/pci.h"

#include "api/exec.h"
#include "api/syscall.h"

#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/fcntl.h"
#include "fs/stat.h"

#include "test/kshell/kshell.h"
#include "test/s5fs_test.h"

GDB_DEFINE_HOOK(boot)
GDB_DEFINE_HOOK(initialized)
GDB_DEFINE_HOOK(shutdown)

static void      *bootstrap(int arg1, void *arg2);
static void      *idleproc_run(int arg1, void *arg2);
static kthread_t *initproc_create(void);
static void      *initproc_run(int arg1, void *arg2);
static void       hard_shutdown(void);

static context_t bootstrap_context;
extern int gdb_wait;
// for self check
static kmutex_t test_mutex;

// testing - kernel 1
extern void *sunghan_test(int, void*);
extern void *sunghan_deadlock_test(int, void*);
extern void *faber_thread_test(int arg1, void *arg2);

// testing - kernel 2
extern void *vfstest_main(int, void*);
extern int faber_fs_thread_test(kshell_t *ksh, int argc, char **argv);
extern int faber_directory_test(kshell_t *ksh, int argc, char **argv);

// ksh
extern void kprintf(kshell_t *ksh, const char *fmt, ...);

// kernel 1 self check
void *mutex_cancel_test(int arg1, void *arg2);
void *t1_lock_mutex(int arg1, void *arg2) ;
void *t2_cancel_mutex(int arg1, void *arg2);

// kernel 2 self check
void *rename_test(int arg1, void *arg2);

int my_vfstest_main(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	// can work directly without using a new thread
	//vfstest_main(1, NULL);
	proc_t *test_proc = proc_create("vfstest_main");
	kthread_t *test_thr = kthread_create(test_proc, vfstest_main, 1, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;	
}

int my_mutex_cancel_test(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	proc_t *test_proc = proc_create("mutex_cancel_test");
	kthread_t *test_thr = kthread_create(test_proc, mutex_cancel_test, 0, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;
}

int my_rename_test(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	proc_t *test_proc = proc_create("rename_test");
	kthread_t *test_thr = kthread_create(test_proc, rename_test, 0, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;	
}

void *mutex_cancel_test(int arg1, void *arg2) {
    int rv;
    pid_t pid;
	dbg(DBG_TEMP, "mutex_cancel_test() started\n");

	proc_t *p2 = proc_create("thread2");
	kthread_t *t2 = kthread_create(p2, t2_cancel_mutex, 0, NULL);
	
	proc_t *p1 = proc_create("thread1");
	kthread_t *t1 = kthread_create(p1, t1_lock_mutex, 0, t2);
	kmutex_init(&test_mutex);
   sched_make_runnable(t1);
   sched_make_runnable(t2);

	while (do_waitpid(-1, 0, &rv) != -ECHILD); // wait for all child to die
	dbg(DBG_TEMP, "mutex_cancel_test() terminated\n");

   return NULL;
	/*
	thread1 lock mutex
	switch 
 	unlock
	cancel thread 2
	exit (switch)

	thread2 cancellable lock mutex - blocking (switch)
   check expected cancelled
	exit (switch)
	*/
}

void *rename_test(int arg1, void *arg2) {
	int rv, fd;
	dbg(DBG_TEMP, "rename_test() started\n");
	if ((fd = do_open("myfile1", O_CREAT | O_RDWR )) != 0){
		dbg(DBG_TEMP, "Self check: Can't create myfile1?\n");
		do_exit(arg1);
		return NULL;
	}
	if (do_rename("myfile1", "myRenamedFile") != 0){
		dbg(DBG_TEMP, "Self check: Can't create myfile1?\n");
		do_exit(arg1);
		return NULL;
	}
	if (do_close(fd) != 0){
		dbg(DBG_TEMP, "Self check: Can't close myRenamedFile?\n");
		do_exit(arg1);
		return NULL;
	}
	if (do_unlink("myRenamedFile") != 0){
		dbg(DBG_TEMP, "Self check: Can't delete myRenamedFile?\n");
		do_exit(arg1);
		return NULL;
	}

	dbg(DBG_TEMP, "rename_test() terminated successfully\n");
	return NULL;
}

void *t1_lock_mutex(int arg1, void *arg2) {
	kmutex_lock(&test_mutex);
   sched_make_runnable(curthr);
	sched_switch();
	kmutex_unlock(&test_mutex);
	sched_cancel(arg2); // kassert doesn't allow adding the thread on runq to add to runq again
//	((kthread_t*)arg2)->kt_cancelled = 1;
	do_exit(arg1);
	return NULL;
}

void *t2_cancel_mutex(int arg1, void *arg2) {
	if ( kmutex_lock_cancellable(&test_mutex) ) {
	   //sched_switch(); // since both unlock and sched_cancel will move it to runq
		do_exit(arg1);
		return NULL;
	}
//	sched_switch(); // since both unlock and sched_cancel will move it to runq
	dbg(DBG_TEMP, "Self check: Mutex not cancelled? %d", curproc->p_pid);
	do_exit(arg1);
	return NULL;
}

int my_faber_thread_test(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	proc_t *test_proc = proc_create("faber_test");
	kthread_t *test_thr = kthread_create(test_proc, faber_thread_test, 0, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;
}

int my_sunghan_test(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	proc_t *test_proc = proc_create("sunghan_test");
	kthread_t *test_thr = kthread_create(test_proc, sunghan_test, 0, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;
}
int my_sunghan_deadlock_test(kshell_t *ksh, int argc, char **argv){
	int rv;
	if (argc != 1){
		kprintf(ksh, "kshell: invalid command - doesn't take argument\n");
		return -1;
	}
	proc_t *test_proc = proc_create("sunghan_deadlock_test");
	kthread_t *test_thr = kthread_create(test_proc, sunghan_deadlock_test, 0, NULL);
	sched_make_runnable(test_thr);
	do_waitpid(test_proc->p_pid, 0, &rv);
	return 0;
}




/**
 * This is the first real C function ever called. It performs a lot of
 * hardware-specific initialization, then creates a pseudo-context to
 * execute the bootstrap function in.
 */
void
kmain()
{
        GDB_CALL_HOOK(boot);

        dbg_init();
        dbgq(DBG_CORE, "Kernel binary:\n");
        dbgq(DBG_CORE, "  text: 0x%p-0x%p\n", &kernel_start_text, &kernel_end_text);
        dbgq(DBG_CORE, "  data: 0x%p-0x%p\n", &kernel_start_data, &kernel_end_data);
        dbgq(DBG_CORE, "  bss:  0x%p-0x%p\n", &kernel_start_bss, &kernel_end_bss);

        page_init();

        pt_init();
        slab_init();
        pframe_init();

        acpi_init();
        apic_init();
        pci_init();
        intr_init();

        gdt_init();

        /* initialize slab allocators */
#ifdef __VM__
        anon_init();
        shadow_init();
#endif
        vmmap_init();
        proc_init();
        kthread_init();

#ifdef __DRIVERS__
        bytedev_init();
        blockdev_init();
#endif

        void *bstack = page_alloc();
        pagedir_t *bpdir = pt_get();
        KASSERT(NULL != bstack && "Ran out of memory while booting.");
        /* This little loop gives gdb a place to synch up with weenix.  In the
         * past the weenix command started qemu was started with -S which
         * allowed gdb to connect and start before the boot loader ran, but
         * since then a bug has appeared where breakpoints fail if gdb connects
         * before the boot loader runs.  See
         *
         * https://bugs.launchpad.net/qemu/+bug/526653
         *
         * This loop (along with an additional command in init.gdb setting
         * gdb_wait to 0) sticks weenix at a known place so gdb can join a
         * running weenix, set gdb_wait to zero  and catch the breakpoint in
         * bootstrap below.  See Config.mk for how to set GDBWAIT correctly.
         *
         * DANGER: if GDBWAIT != 0, and gdb is not running, this loop will never
         * exit and weenix will not run.  Make SURE the GDBWAIT is set the way
         * you expect.
         */
        while (gdb_wait) ;
        context_setup(&bootstrap_context, bootstrap, 0, NULL, bstack, PAGE_SIZE, bpdir);
        context_make_active(&bootstrap_context);

        panic("\nReturned to kmain()!!!\n");
}

/**
 * Clears all interrupts and halts, meaning that we will never run
 * again.
 */
static void
hard_shutdown()
{
#ifdef __DRIVERS__
        vt_print_shutdown();
#endif
        __asm__ volatile("cli; hlt");
}

/**
 * This function is called from kmain, however it is not running in a
 * thread context yet. It should create the idle process which will
 * start executing idleproc_run() in a real thread context.  To start
 * executing in the new process's context call context_make_active(),
 * passing in the appropriate context. This function should _NOT_
 * return.
 *
 * Note: Don't forget to set curproc and curthr appropriately.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
bootstrap(int arg1, void *arg2)
{
        /* If the next line is removed/altered in your submission, 20 points will be deducted. */
        dbgq(DBG_TEST, "SIGNATURE: 53616c7465645f5f769ed398e006f876d21871b23f6235fab6d6dcb557ef32a7da5d3d6d394c01c02d9f029726127dda\n");
        /* necessary to finalize page table information */
        pt_template_init();


        //NOT_YET_IMPLEMENTED("PROCS: bootstrap");
		  // build a thread context, create thread, run by context_make_active	
		  curproc = proc_create("idle"); // dw question: anything var needs to refer to idle process?
		  
		  KASSERT(NULL != curproc); /* curproc was uninitialized before, it is initialized here to point to the "idle" process */
		  KASSERT(PID_IDLE == curproc->p_pid); /* make sure the process ID of the created "idle" process is PID_IDLE */
		  dbg(DBG_PRINT, "(GRADING1A 1.a)\n");
		  //curproc->p_pagedir = bootstrap_context.c_pdptr;
		  curthr  = kthread_create(curproc, idleproc_run, 0, NULL);
		  KASSERT(NULL != curthr); /* curthr was uninitialized before, it is initialized here to point to the thread of the "idle" process */
		  dbg(DBG_PRINT, "(GRADING1A 1.a)\n");
		  
		  //context_setup(&(curthr->kt_ctx), idleproc_run, 0, NULL, (void*)(bootstrap_context.c_kstack), bootstrap_context.c_kstacksz, bootstrap_context.c_pdptr);
        //context_make_active(&(curthr->kt_ctx));
		  //context_setup(&bootstrap_context, idleproc_run, 0, NULL, (void*)(bootstrap_context.c_kstack), bootstrap_context.c_kstacksz, bootstrap_context.c_pdptr);
        context_make_active(&(curthr->kt_ctx));
	
        panic("weenix returned to bootstrap()!!! BAD!!!\n");
        return NULL;
}

/**
 * Once we're inside of idleproc_run(), we are executing in the context of the
 * first process-- a real context, so we can finally begin running
 * meaningful code.
 *
 * This is the body of process 0. It should initialize all that we didn't
 * already initialize in kmain(), launch the init process (initproc_run),
 * wait for the init process to exit, then halt the machine.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
idleproc_run(int arg1, void *arg2)
{
        int status;
        pid_t child;

        /* create init proc */
        kthread_t *initthr = initproc_create();
        init_call_all();
        GDB_CALL_HOOK(initialized);

        /* Create other kernel threads (in order) */

#ifdef __VFS__
        /* Once you have VFS remember to set the current working directory
         * of the idle and init processes */
        //NOT_YET_IMPLEMENTED("VFS: idleproc_run");
		  curproc->p_cwd = vfs_root_vn;
		  vref(vfs_root_vn);
		  initthr->kt_proc->p_cwd = vfs_root_vn;
		  vref(vfs_root_vn);
		  // TODO: Kernel 3 needs to set pageoutd p_cwd

        /* Here you need to make the null, zero, and tty devices using mknod */
        /* You can't do this until you have VFS, check the include/drivers/dev.h
         * file for macros with the device ID's you will need to pass to mknod */
        //NOT_YET_IMPLEMENTED("VFS: idleproc_run");
		  //vnode_t *temp = NULL, *temp2 = NULL;
		  do_mkdir("/dev");
		  do_mknod("/dev/null", S_IFCHR, MEM_NULL_DEVID);
		  do_mknod("/dev/zero", S_IFCHR, MEM_ZERO_DEVID);
		  do_mknod("/dev/tty0", S_IFCHR, MKDEVID(2, 0));
		  do_mknod("/dev/tty1", S_IFCHR, MKDEVID(2, 1));
		  do_mknod("/dev/tty2", S_IFCHR, MKDEVID(2, 2));
		  dbg(DBG_PRINT, "(GRADING2A)\n"); 

#endif
// can't access pageoutd
//#ifdef __S5FS__
//	pageoutd->p_cwd = vfs_root_vn;
//	vref(vfs_root_vn);
//#endif 
        /* Finally, enable interrupts (we want to make sure interrupts
         * are enabled AFTER all drivers are initialized) */
        intr_enable();

        /* Run initproc */
        sched_make_runnable(initthr);
        /* Now wait for it */
        child = do_waitpid(-1, 0, &status);
        KASSERT(PID_INIT == child);

#ifdef __MTP__
        kthread_reapd_shutdown();
#endif


#ifdef __SHADOWD__
        /* wait for shadowd to shutdown */
        shadowd_shutdown();
#endif

#ifdef __VFS__
        /* Shutdown the vfs: */
        dbg_print("weenix: vfs shutdown...\n");
        vput(curproc->p_cwd);
        if (vfs_shutdown())
                panic("vfs shutdown FAILED!!\n");

#endif

        /* Shutdown the pframe system */
#ifdef __S5FS__
        pframe_shutdown();
#endif

        dbg_print("\nweenix: halted cleanly!\n");
        GDB_CALL_HOOK(shutdown);
        hard_shutdown();
        return NULL;
}

/**
 * This function, called by the idle process (within 'idleproc_run'), creates the
 * process commonly refered to as the "init" process, which should have PID 1.
 *
 * The init process should contain a thread which begins execution in
 * initproc_run().
 *
 * @return a pointer to a newly created thread which will execute
 * initproc_run when it begins executing
 */
static kthread_t *
initproc_create(void)
{		  
        //NOT_YET_IMPLEMENTED("PROCS: initproc_create");
        //return NULL;
		  proc_t *p = proc_create("init");
		  
		  kthread_t *thr = kthread_create(p, initproc_run, 0, NULL);
/* p is the pointer to the "init" process, thr is the pointer to the thread of p */
        KASSERT(NULL != p); 
        //dbg(DBG_PRINT, "GRADING1A 1.b\n"); 
        KASSERT(PID_INIT == p->p_pid);
        //dbg(DBG_PRINT, "GRADING1A 1.b\n"); 
        KASSERT(NULL != thr);
        dbg(DBG_PRINT, "(GRADING1A 1.b)\n"); 
		  
		  return thr;
}


/**
 * The init thread's function changes depending on how far along your Weenix is
 * developed. Before VM/FI, you'll probably just want to have this run whatever
 * tests you've written (possibly in a new process). After VM/FI, you'll just
 * exec "/sbin/init".
 *
 * Both arguments are unused.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
initproc_run(int arg1, void *arg2)
{
//        NOT_YET_IMPLEMENTED("PROCS: initproc_run");
// note: create a process/thread to run faber test
//int rv;
//proc_t *test_proc = proc_create("faber_test");
//kthread_t *test_thr = kthread_create(test_proc, faber_thread_test, arg1, arg2);
//proc_t *test_proc = proc_create("sunghan_test");
//kthread_t *test_thr = kthread_create(test_proc, sunghan_test, arg1, arg2);
//proc_t *test_proc = proc_create("sunghan_deadlock_test");
//kthread_t *test_thr = kthread_create(test_proc, sunghan_deadlock_test, arg1, arg2);
//sched_make_runnable(test_thr);
//do_waitpid(-1, 0, &rv);
//dbg(DBG_TEST, "%s (%d) exited: %d\n", test_proc->p_comm, test_proc->p_pid, rv);

//proc_t *test_proc = proc_create("mutex_cancel_test");
//kthread_t *test_thr = kthread_create(test_proc, mutex_cancel_test, arg1, arg2);
//sched_make_runnable(test_thr);
//do_waitpid(-1, 0, &rv);
//dbg(DBG_TEST, "%s (%d) exited: %d\n", test_proc->p_comm, test_proc->p_pid, rv);
#ifdef __VM__
    char *argv[6] = { NULL }; // changed from empty
    char *envp[] = { NULL };
	 #if CS402INITCHOICE > 0
	 	#if CS402INITCHOICE > 1	
			do_open("/dev/tty0", O_RDONLY); // stdin
			do_open("/dev/tty0", O_WRONLY); // stdout
	 		#if CS402INITCHOICE > 2
	 			#if CS402INITCHOICE > 3
	 				#if CS402INITCHOICE > 4
	 					#if CS402INITCHOICE > 5
							#if CS402INITCHOICE > 6 
	 							argv[0] = "/usr/bin/args";
	 							argv[1] = "ab";
	 							argv[2] = "cde"; 
	 							argv[3] = "fghi"; 
	 							argv[4] = "j"; 
	 							argv[5] = NULL ;
    							kernel_execve("/usr/bin/args", argv, envp);
							#else // 6
	 							argv[0] = "/usr/bin/fork-and-wait"; 
	 							argv[1] = NULL ;
    							kernel_execve("/usr/bin/fork-and-wait", argv, envp);
							#endif
						#else // 5
	 						argv[0] = "/bin/stat"; 
	 						argv[1] = "/usr"; 
	 						argv[2] = NULL ;
    						kernel_execve("/bin/stat", argv, envp);
						#endif
	 				#else // 4 
	 					argv[0] = "/bin/ls"; 
	 					argv[1] = "/usr/bin"; 
	 					argv[2] = NULL ;
    					kernel_execve("/bin/ls", argv, envp);
					#endif
				#else // 3
	 				argv[0] = "/bin/stat"; 
	 				argv[1] = "/README"; 
	 				argv[2] = NULL ;
    				kernel_execve("/bin/stat", argv, envp);
				#endif
			#else // 2
	 			argv[0] = "/bin/uname"; 
	 			argv[1] = "-a"; 
	 			argv[2] = NULL ;
    			kernel_execve("/bin/uname", argv, envp);
			#endif
		#else // 1
			argv[0] = NULL; 
			kernel_execve("/sbin/init", argv, envp);
		#endif
	 #else // 0
    	kernel_execve("/usr/bin/hello", argv, envp);
    	//kernel_execve("/usr/bin/fork-and-wait", argv, envp);
	 #endif

/*
    #if CS402INITCHOICE > 0
      #if CS402INITCHOICE > 1
        #if CS402INITCHOICE > 2
          #if CS402INITCHOICE > 3
            #if CS402INITCHOICE > 4
              kernel_execve("/usr/bin/memtest", argvec, envvec);
            #else
              kernel_execve("/usr/bin/vfstest", argvec, envvec);
            #endif
          #else
            kernel_execve("/bin/uname", argvec, envvec);
          #endif
        #else
          kernel_execve("/usr/bin/fork-and-wait", argvec, envvec);
        #endif
      #else
        kernel_execve("/usr/bin/hello", argvec, envvec);
      #endif
    #else
      kernel_execve("/sbin/init", argvec, envvec);
    #endif
*/

#else
    #ifdef __DRIVERS__
		 kshell_add_command("sunghan", my_sunghan_test, "Run sunghan_test().");
	    kshell_add_command("deadlock", my_sunghan_deadlock_test, "Run sunghan_deadlock_test().");
	    kshell_add_command("faber", my_faber_thread_test, "Run faber_thread_test().");
	    kshell_add_command("cmutex", my_mutex_cancel_test, "Self check: mutex_cancel_test().");
		 #ifdef __VFS__
		     kshell_add_command("vfstest_main", my_vfstest_main, "Run my_vfstest_main().");
		     kshell_add_command("thrtest", faber_fs_thread_test, "Run faber_fs_thread_test().");
    		  kshell_add_command("dirtest", faber_directory_test, "Run faber_directory_test().");
	    	  kshell_add_command("vfs_rename", my_rename_test, "Self check: rename_test().");
		  	  dbg(DBG_PRINT, "(GRADING2A)\n");
		 #endif
        kshell_t *kshell = kshell_create(0);
        //if (NULL == kshell)  panic("init: Couldn't create kernel shell\n");  
		  KASSERT(kshell != NULL);
		  dbg(DBG_PRINT, "(GRADING1A)\n");
        while (kshell_execute_next(kshell));// { dbg(DBG_PRINT, "(GRADING1B)\n"); } ;  
        kshell_destroy(kshell);
		  dbg(DBG_PRINT, "(GRADING1A)\n");
    #endif /* __DRIVERS__ */
#endif

		  dbg(DBG_PRINT, "(GRADING1A)\n"); 
        return NULL;
}
