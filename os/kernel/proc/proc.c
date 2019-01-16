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

#include "kernel.h"
#include "config.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "proc/kthread.h"
#include "proc/proc.h"
#include "proc/sched.h"
#include "proc/proc.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/mm.h"
#include "mm/mman.h"

#include "vm/vmmap.h"

#include "fs/vfs.h"
#include "fs/vfs_syscall.h"
#include "fs/vnode.h"
#include "fs/file.h"

proc_t *curproc = NULL; /* global */
static slab_allocator_t *proc_allocator = NULL;

static list_t _proc_list;
static proc_t *proc_initproc = NULL; /* Pointer to the init process (PID 1) */

//extern void
//ktqueue_enqueue(ktqueue_t *q, kthread_t *thr);

void
proc_init()
{
        list_init(&_proc_list);
        proc_allocator = slab_allocator_create("proc", sizeof(proc_t));
        KASSERT(proc_allocator != NULL);
}

proc_t *
proc_lookup(int pid)
{
        proc_t *p;
        list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                if (p->p_pid == pid) {
                        return p;
                }
        } list_iterate_end();
        return NULL;
}

list_t *
proc_list()
{
        return &_proc_list;
}

size_t
proc_info(const void *arg, char *buf, size_t osize)
{
        const proc_t *p = (proc_t *) arg;
        size_t size = osize;
        proc_t *child;

        KASSERT(NULL != p);
        KASSERT(NULL != buf);

        iprintf(&buf, &size, "pid:          %i\n", p->p_pid);
        iprintf(&buf, &size, "name:         %s\n", p->p_comm);
        if (NULL != p->p_pproc) {
                iprintf(&buf, &size, "parent:       %i (%s)\n",
                        p->p_pproc->p_pid, p->p_pproc->p_comm);
        } else {
                iprintf(&buf, &size, "parent:       -\n");
        }

#ifdef __MTP__
        int count = 0;
        kthread_t *kthr;
        list_iterate_begin(&p->p_threads, kthr, kthread_t, kt_plink) {
                ++count;
        } list_iterate_end();
        iprintf(&buf, &size, "thread count: %i\n", count);
#endif

        if (list_empty(&p->p_children)) {
                iprintf(&buf, &size, "children:     -\n");
        } else {
                iprintf(&buf, &size, "children:\n");
        }
        list_iterate_begin(&p->p_children, child, proc_t, p_child_link) {
                iprintf(&buf, &size, "     %i (%s)\n", child->p_pid, child->p_comm);
        } list_iterate_end();

        iprintf(&buf, &size, "status:       %i\n", p->p_status);
        iprintf(&buf, &size, "state:        %i\n", p->p_state);

#ifdef __VFS__
#ifdef __GETCWD__
        if (NULL != p->p_cwd) {
                char cwd[256];
                lookup_dirpath(p->p_cwd, cwd, sizeof(cwd));
                iprintf(&buf, &size, "cwd:          %-s\n", cwd);
        } else {
                iprintf(&buf, &size, "cwd:          -\n");
        }
#endif /* __GETCWD__ */
#endif

#ifdef __VM__
        iprintf(&buf, &size, "start brk:    0x%p\n", p->p_start_brk);
        iprintf(&buf, &size, "brk:          0x%p\n", p->p_brk);
#endif

        return size;
}

size_t
proc_list_info(const void *arg, char *buf, size_t osize)
{
        size_t size = osize;
        proc_t *p;

        KASSERT(NULL == arg);
        KASSERT(NULL != buf);

#if defined(__VFS__) && defined(__GETCWD__)
        iprintf(&buf, &size, "%5s %-13s %-18s %-s\n", "PID", "NAME", "PARENT", "CWD");
#else
        iprintf(&buf, &size, "%5s %-13s %-s\n", "PID", "NAME", "PARENT");
#endif

        list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                char parent[64];
                if (NULL != p->p_pproc) {
                        snprintf(parent, sizeof(parent),
                                 "%3i (%s)", p->p_pproc->p_pid, p->p_pproc->p_comm);
                } else {
                        snprintf(parent, sizeof(parent), "  -");
                }

#if defined(__VFS__) && defined(__GETCWD__)
                if (NULL != p->p_cwd) {
                        char cwd[256];
                        lookup_dirpath(p->p_cwd, cwd, sizeof(cwd));
                        iprintf(&buf, &size, " %3i  %-13s %-18s %-s\n",
                                p->p_pid, p->p_comm, parent, cwd);
                } else {
                        iprintf(&buf, &size, " %3i  %-13s %-18s -\n",
                                p->p_pid, p->p_comm, parent);
                }
#else
                iprintf(&buf, &size, " %3i  %-13s %-s\n",
                        p->p_pid, p->p_comm, parent);
#endif
        } list_iterate_end();
        return size;
}

static pid_t next_pid = 0;

/**
 * Returns the next available PID.
 *
 * Note: Where n is the number of running processes, this algorithm is
 * worst case O(n^2). As long as PIDs never wrap around it is O(n).
 *
 * @return the next available PID
 */
static int
_proc_getid()
{
        proc_t *p;
        pid_t pid = next_pid;
        while (1) {
failed:
                list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                        if (p->p_pid == pid) {
                                if ((pid = (pid + 1) % PROC_MAX_COUNT) == next_pid) {
                                        return -1;
                                } else {
                                        goto failed;
                                }
                        }
                } list_iterate_end();
                next_pid = (pid + 1) % PROC_MAX_COUNT;
                return pid;
        }
}

/*
 * The new process, although it isn't really running since it has no
 * threads, should be in the PROC_RUNNING state.
 *
 * Don't forget to set proc_initproc when you create the init
 * process. You will need to be able to reference the init process
 * when reparenting processes to the init process.
 */
proc_t *
proc_create(char *name)
{
        //NOT_YET_IMPLEMENTED("PROCS: proc_create");
        //return NULL;
		  pid_t pid;
		  //proc_t *pChild = NULL;
		  proc_t *proc_ptr = (proc_t *) slab_obj_alloc(proc_allocator);
		  KASSERT(proc_ptr != NULL); // dw question: do we need to add additional kassert?
		  dbg(DBG_PRINT, "(GRADING1A)\n");

		  // TODO dbg conforming 
		  list_init(&(proc_ptr->p_threads));
		  list_init(&(proc_ptr->p_children));

		  if (!curproc) { // meaning no running process
			  proc_ptr->p_pproc = NULL; // idle process
			
		      dbg(DBG_PRINT, "(GRADING1A)\n");

		  }
		  else { // when generating init or other processes
			  proc_ptr->p_pproc = curproc;
#ifdef __VFS__
			  if (curproc->p_pid != PID_IDLE){
			  		proc_ptr->p_cwd = curproc->p_cwd;
			  		vref(curproc->p_cwd);
					dbg(DBG_PRINT, "(GRADING2B)\n");
			  }
			  //else {
			  // 	proc_ptr->p_cwd = NULL;
			  // 	dbg(DBG_PRINT, "(NEWPATH)\n");
			  //}
			  dbg(DBG_PRINT, "(GRADING2A)\n");
#endif			  
			  list_insert_head(&(curproc->p_children), &(proc_ptr->p_child_link));
		     dbg(DBG_PRINT, "(GRADING1A)\n");
		  }
		  pid = _proc_getid();
		  proc_ptr->p_pid = pid;
		  KASSERT(pid >= 0);
		  dbg(DBG_PRINT, "(GRADING1A)\n");  
/* pid is the process ID of the process that will be created in this function */
		KASSERT(PID_IDLE != pid || list_empty(&_proc_list)); /* pid can only be PID_IDLE if this is the first process */
		KASSERT(PID_INIT != pid || PID_IDLE == curproc->p_pid); /* pid can only be PID_INIT if the running process is the "idle" process */
		  dbg(DBG_PRINT, "(GRADING1A 2.a)\n");
		  list_insert_head(&_proc_list, &(proc_ptr->p_list_link));
		if (pid == PID_INIT){
			proc_initproc = proc_ptr;
		   dbg(DBG_PRINT, "(GRADING1A)\n");
		}

		  strncpy( proc_ptr->p_comm, name, PROC_NAME_LEN);

		  sched_queue_init(&(proc_ptr->p_wait));
		  proc_ptr->p_pagedir = pt_create_pagedir();
		  proc_ptr->p_status = 0;
		  proc_ptr->p_state = PROC_RUNNING;
		  
//		  proc_ptr->p_files = NULL;
#ifdef __VFS__

		  for (int index = 0; index < NFILES; index++) {
      		proc_ptr->p_files[index] = NULL;
				dbg(DBG_PRINT, "(GRADING2A)\n");

        }
			  dbg(DBG_PRINT, "(GRADING2A)\n"); 
		  
#endif

		  proc_ptr->p_brk = (void*)USER_MEM_LOW;      
		  proc_ptr->p_start_brk = (void*)USER_MEM_LOW;
#ifdef __VM__		  
		  proc_ptr->p_vmmap = vmmap_create() ;
		  proc_ptr->p_vmmap->vmm_proc = proc_ptr;
		  dbg(DBG_PRINT, "(GRADING3A)\n");
#else
		  proc_ptr->p_vmmap = NULL;
		  dbg(DBG_PRINT, "(GRADING1A)\n");
#endif
		  dbg(DBG_PRINT, "(GRADING1A)\n");
		  return proc_ptr;
}

/**
 * Cleans up as much as the process as can be done from within the
 * process. This involves:
 *    - Closing all open files (VFS)
 *    - Cleaning up VM mappings (VM)
 *    - Waking up its parent if it is waiting
 *    - Reparenting any children to the init process
 *    - Setting its status and state appropriately
 *
 * The parent will finish destroying the process within do_waitpid (make
 * sure you understand why it cannot be done here). Until the parent
 * finishes destroying it, the process is informally called a 'zombie'
 * process.
 *
 * This is also where any children of the current process should be
 * reparented to the init process (unless, of course, the current
 * process is the init process. However, the init process should not
 * have any children at the time it exits).
 *
 * Note: You do _NOT_ have to special case the idle process. It should
 * never exit this way.
 *
 * @param status the status to exit the process with
 */
void
proc_cleanup(int status)
{
        // NOT_YET_IMPLEMENTED("PROCS: proc_cleanup");

        /* "init" process must exist and proc_initproc initialized */
        KASSERT(NULL != proc_initproc); 
        /* this process must not be "idle" process */
        // since idle is terminated by shutdown()
        KASSERT(1 <= curproc->p_pid); 
        /* this process must have a parent when this function is entered */
        KASSERT(NULL != curproc->p_pproc); 
		  dbg(DBG_PRINT, "(GRADING1A 2.b)\n");

        proc_t * p = NULL;
        
#ifdef __VFS__
        // TO DO in kernel 2
        // closing all open files (VFS)
		  for (int index = 0; index < NFILES; index++) {
      		// not reached - keep for kernel 3
#ifdef __VM__
				if (curproc->p_files[index] != NULL){
					do_close(index);
					dbg(DBG_PRINT, "(GRADING3A)\n");
				}
#endif						
				KASSERT(curproc->p_files[index] == NULL);
				dbg(DBG_PRINT, "(GRADING2A)\n");
        }
			//if (curproc->p_cwd != NULL){
			// only vput for all child processes except pageoutd
			if (curproc->p_pid == PID_INIT || curproc->p_pid == PID_IDLE || 
				(curproc->p_pid != PID_INIT && curproc->p_pid != PID_IDLE && curproc->p_pproc->p_pid != PID_IDLE) )	{
			  vput(curproc->p_cwd);
			  dbg(DBG_PRINT, "(GRADING3A)\n"); 
			}
			// another way
			//if (curproc->p_cwd != NULL){
			//  vput(curproc->p_cwd);
			//  dbg(DBG_PRINT, "(NEWPATH)\n"); 
			//}

		  dbg(DBG_PRINT, "(GRADING2A)\n"); 
#endif
        // TO DO in kernel 3
        // cleaning up VM mappings (VM)
#ifdef __VM__
		  if (curproc->p_vmmap != NULL){
			  vmmap_destroy(curproc->p_vmmap);
		  	  dbg(DBG_PRINT, "(GRADING3A)\n");
		  }
		  dbg(DBG_PRINT, "(GRADING3A)\n");
#endif

        // Waking up its parent if it is waiting
        // dennis tip: sched_wakeup_on to wake up the parent 
        // and call sched_make_runnable
        sched_wakeup_on(&(curproc->p_pproc->p_wait));


        // reparenting any children to the init processes
        list_iterate_begin(&curproc->p_children, p, proc_t, p_child_link) {
            list_insert_head(&(proc_initproc->p_children), &(p->p_child_link));
		      dbg(DBG_PRINT, "(GRADING1C)\n");
        } list_iterate_end();

        // setting its status and states appropriately
        curproc->p_status = status;
        curproc->p_state = PROC_DEAD;
        
        // remove curproc from _proc_list for debug
        //list_remove(&(curproc->p_list_link));

        /* this process must still have a parent when this function returns */
        KASSERT(NULL != curproc->p_pproc);  
		  dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
	     
		  curthr->kt_state = KT_EXITED;

        /* the thread in this process should be in the KT_EXITED 
        state when this function returns */
        KASSERT(KT_EXITED == curthr->kt_state); 
		  dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
         
}   


/*
 * This has nothing to do with signals and kill(1).
 *
 * Calling this on the current process is equivalent to calling
 * do_exit().
 *
 * In Weenix, this is only called from proc_kill_all.
 */
void
proc_kill(proc_t *p, int status)
{
        //NOT_YET_IMPLEMENTED("PROCS: proc_kill");
		  if (p == curproc){
			  dbg(DBG_PRINT, "(GRADING1C)\n");
			  do_exit(status);
		  }
		  else{
			  kthread_cancel(  list_head(&(p->p_threads), kthread_t, kt_plink), (void *)status );
			  dbg(DBG_PRINT, "(GRADING1C)\n");
		  }
}

/*
 * Remember, proc_kill on the current process will _NOT_ return.
 * Don't kill direct children of the idle process.
 *
 * In Weenix, this is only called by sys_halt.
 */
void
proc_kill_all()
{
        //NOT_YET_IMPLEMENTED("PROCS: proc_kill_all");
		  proc_t *p;
		  /*
		  if (curproc->p_pid != PID_IDLE){
				list_iterate_begin(&(curproc->p_children), p, proc_t, p_child_link) {
					proc_kill(p, -1); // TODO: check exit value
				} list_iterate_end();
				if (curproc->p_pid != PID_INIT)
					proc_kill(curproc, -1);
		  }
		  */
		  dbg(DBG_PRINT, "(GRADING1C)\n");
		  // NOTE: newest process is inserted at the beginning of the list, so when we kill we will start from the (newer) child process to (older) parent process
		  list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
			  if (p->p_pid != PID_IDLE &&  (p->p_pproc != NULL && p->p_pproc->p_pid != PID_IDLE)){
				  if (p != curproc){
					proc_kill(p, -1); // TODO: check exit value
				    dbg(DBG_PRINT, "(GRADING1C)\n");
				  }
				    dbg(DBG_PRINT, "(GRADING3A)\n");
			  }
			  dbg(DBG_PRINT, "(GRADING1C)\n");
		  } list_iterate_end();
		  //not reached//if (curproc->p_pid != PID_IDLE && (p->p_pproc != NULL && curproc->p_pproc->p_pid != PID_IDLE)){
		  //not reached//   dbg(DBG_PRINT, "(NEWPATH)\n");
		  //not reached// 	proc_kill(curproc, -1); // doesn't return;
		  //not reached//}
			  dbg(DBG_PRINT, "(GRADING3A)\n");

}

/*
 * This function is only called from kthread_exit.
 *
 * Unless you are implementing MTP, this just means that the process
 * needs to be cleaned up and a new thread needs to be scheduled to
 * run. If you are implementing MTP, a single thread exiting does not
 * necessarily mean that the process should be exited.
 */
void
proc_thread_exited(void *retval)
{
        // NOT_YET_IMPLEMENTED("PROCS: proc_thread_exited");

        // pseudo code
        // step 1: the process needs to be cleaned up 
        // not sure about the "status" parameter for the function
        // proc_cleanup(int status)
        proc_cleanup((int)retval);
		  dbg(DBG_PRINT, "(GRADING1A)\n");

        
        // steps 2: a new threads needs to be scheduled to run
        sched_switch();
        
}
/* If pid is -1 dispose of one of the exited children of the current
 * process and return its exit status in the status argument, or if
 * all children of this process are still running, then this function
 * blocks on its own p_wait queue until one exits.
 *
 * If pid is greater than 0 and the given pid is a child of the
 * current process then wait for the given pid to exit and dispose
 * of it.
 *
 * If the current process has no children, or the given pid is not
 * a child of the current process return -ECHILD.
 *
 * Pids other than -1 and positive numbers are not supported.
 * Options other than 0 are not supported.
 */
pid_t
do_waitpid(pid_t pid, int options, int *status)
{
        //NOT_YET_IMPLEMENTED("PROCS: do_waitpid");
		  // TODO: need to review no supported cases with kassert
		  KASSERT (pid == -1 || pid > 0);
		  KASSERT (options == 0);
          dbg(DBG_PRINT, "(GRADING1A)\n");
		  proc_t *p = NULL; // pointer to dead child process
		  if (list_empty(&(curproc->p_children))) {// no child process
            dbg(DBG_PRINT, "(GRADING1C)\n");
				return -ECHILD;
		  }
		  if (pid == -1) {
			  // check dead child first
           list_iterate_begin(&(curproc->p_children), p, proc_t, p_child_link) {
                if (p->p_state == PROC_DEAD) {
						 if (status != NULL){ // k3
						   *status = p->p_status;
            			dbg(DBG_PRINT, "(GRADING3A)\n");
						 }
							list_remove(&(p->p_child_link)); 
							
							KASSERT(NULL != p); /* must have found a dead child process */
							//dbg(DBG_PRINT, "(GRADING1C)\n");
							KASSERT(-1 == pid || p->p_pid == pid); /* if the pid argument is not -1, then pid must be the process ID of the found dead child process */
							//dbg(DBG_PRINT, "(GRADING1C)\n");
							KASSERT(NULL != p->p_pagedir); /* this process should have a valid pagedir before you destroy it */
							dbg(DBG_PRINT, "(GRADING1C)\n");
							pt_destroy_pagedir(p->p_pagedir);
							
							kthread_destroy(list_head(&(p->p_threads), kthread_t, kt_plink)); // NOTE: need to save ptr to particular thread if MTP != 0
							//vput(p->p_cwd);
							list_remove(&(p->p_list_link));
						   slab_obj_free(proc_allocator, p);
                     dbg(DBG_PRINT, "(GRADING1C)\n");

                     return p->p_pid; 
                }
            	 dbg(DBG_PRINT, "(GRADING1A)\n");

           } list_iterate_end();
			  // if there is a dead child, it must not reach here
			  sched_sleep_on(&(curproc->p_wait));
			  //ktqueue_enqueue(&(curproc->p_wait), curthr);
			  //sched_switch();
			  // need to get pid of exit child
           list_iterate_begin(&(curproc->p_children), p, proc_t, p_child_link) {
                if (p->p_state == PROC_DEAD) {
						 if (status != NULL){ // k3
						   *status = p->p_status;
            			dbg(DBG_PRINT, "(GRADING3A)\n");
						 } 
							list_remove(&(p->p_child_link));

							KASSERT(NULL != p); /* must have found a dead child process */
							//dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
							KASSERT(-1 == pid || p->p_pid == pid); /* if the pid argument is not -1, then pid must be the process ID of the found dead child process */
							//dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
							KASSERT(NULL != p->p_pagedir); /* this process should have a valid pagedir before you destroy it */
							dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
							pt_destroy_pagedir(p->p_pagedir);

							kthread_destroy(list_head(&(p->p_threads), kthread_t, kt_plink));
							//vput(p->p_cwd);
							list_remove(&(p->p_list_link));
						   slab_obj_free(proc_allocator, p);
            			dbg(DBG_PRINT, "(GRADING1A)\n");
                     return p->p_pid; 
                }
            	 dbg(DBG_PRINT, "(GRADING1A)\n");

           } list_iterate_end();
			  // dbg(DBG_PRINT, "(NEWPATH)\n"); // k1 report - not reached
		  }
		  else if (pid > 0 && (p = proc_lookup(pid))){
           if (p->p_state != PROC_DEAD) {
			  		sched_sleep_on(&(curproc->p_wait));
					//ktqueue_enqueue(&(curproc->p_wait), curthr);
			  		//sched_switch();
            	dbg(DBG_PRINT, "(GRADING1C)\n");
			  }
				if (status != NULL){ // k3
			  		*status = p->p_status;	
            	dbg(DBG_PRINT, "(GRADING3B 7)\n");
				}
			  list_remove(&(p->p_child_link));

				KASSERT(NULL != p); /* must have found a dead child process */
				//dbg(DBG_PRINT, "(GRADING1C)\n");
				KASSERT(-1 == pid || p->p_pid == pid); /* if the pid argument is not -1, then pid must be the process ID of the found dead child process */
				//dbg(DBG_PRINT, "(GRADING1C)\n");
				KASSERT(NULL != p->p_pagedir); /* this process should have a valid pagedir before you destroy it */
				dbg(DBG_PRINT, "(GRADING1C)\n");
				pt_destroy_pagedir(p->p_pagedir);
			  
			  kthread_destroy(list_head(&(p->p_threads), kthread_t, kt_plink));
			  //vput(p->p_cwd);
							list_remove(&(p->p_list_link));
			  slab_obj_free(proc_allocator, p);
           dbg(DBG_PRINT, "(GRADING1C)\n");
           return p->p_pid; 
		  }
		  // non supported case
        dbg(DBG_PRINT, "(GRADING1C)\n");
		  return -ECHILD;
}

/*
 * Cancel all threads and join with them (if supporting MTP), and exit from the current
 * thread.
 *
 * @param status the exit status of the process
 */
void
do_exit(int status)
{
		//int rv = curproc->p_status;
        //NOT_YET_IMPLEMENTED("PROCS: do_exit");
		  //if (curthr->kt_cancelled){
		  //		dbg(DBG_PRINT, "(GRADING1C)\n");
		  //		kthread_cancel(curthr, (void*) rv);
		  //}
		  //else{
			  // Only 1 thread to cancel since MTP = 0
		  	  dbg(DBG_PRINT, "(GRADING1C)\n");
			  kthread_cancel(curthr, (void*) status); // kthread_cancel will call do_exit if it is current thread
		  //}
		  //kthread_exit((void*) status);
		  //curproc->p_status = status;
}
