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

#include "util/debug.h"

#include "proc/proc.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/pagetable.h"

#include "vm/pagefault.h"
#include "vm/vmmap.h"

/*
 * This gets called by _pt_fault_handler in mm/pagetable.c The
 * calling function has already done a lot of error checking for
 * us. In particular it has checked that we are not page faulting
 * while in kernel mode. Make sure you understand why an
 * unexpected page fault in kernel mode is bad in Weenix. You
 * should probably read the _pt_fault_handler function to get a
 * sense of what it is doing.
 *
 * Before you can do anything you need to find the vmarea that
 * contains the address that was faulted on. Make sure to check
 * the permissions on the area to see if the process has
 * permission to do [cause]. If either of these checks does not
 * pass kill the offending process, setting its exit status to
 * EFAULT (normally we would send the SIGSEGV signal, however
 * Weenix does not support signals).
 *
 * Now it is time to find the correct page. Make sure that if the
 * user writes to the page it will be handled correctly. This
 * includes your shadow objects' copy-on-write magic working
 * correctly.
 *
 * Finally call pt_map to have the new mapping placed into the
 * appropriate page table.
 *
 * @param vaddr the address that was accessed to cause the fault
 *
 * @param cause this is the type of operation on the memory
 *              address which caused the fault, possible values
 *              can be found in pagefault.h
 */
void
handle_pagefault(uintptr_t vaddr, uint32_t cause)
{
//        NOT_YET_IMPLEMENTED("VM: handle_pagefault");
	KASSERT( (cause & FAULT_USER) ); // make sure not kernel page fault
	dbg(DBG_PRINT, "(GRADING3A)\n");
	int forwrite = 0;
	pframe_t *pf = NULL;
	// Question: is fault going to be multiple bit fail?
	/*
	0. check permission of the vmarea
	1. find a page frame (and a physical page), 
	2. fill the page frame (to be more precise, the physical page associated with the page frame) with data from disk (if it's associated with a file), 
	3. fix the page table to map the corresponding virtual page to the physical page, and 
	4. update kernel data structures to make everything consistent. 
	5. When the kernel declares that the page fault has been fixed and returns to the user process,
	*/
//	addr_perm(curproc, vaddr, perm);
	vmarea_t *vma = vmmap_lookup(curproc->p_vmmap, ADDR_TO_PN(vaddr));
	if (vma == NULL){
		dbg(DBG_PRINT, "(GRADING3C 5)\n");
		proc_kill(curproc, EFAULT); // killing curproc doesn't return
		//return;
	}
	if ((cause & FAULT_WRITE) &&  !(vma->vma_prot & PROT_WRITE)) { // if this area is shared and read only
		dbg(DBG_PRINT, "(GRADING3D 2)\n");
		proc_kill(curproc, EFAULT); // killing curproc doesn't return
	}
// user fault & no read permission
	if ((cause == FAULT_USER) && !(vma->vma_prot & PROT_READ)) {
		dbg(DBG_PRINT, "(GRADING3D 2)\n");
		proc_kill(curproc, EFAULT); // killing curproc doesn't return
	}
// other cases?	
//	if (cause & FAULT_PRESENT) {
//				
//	}
	mmobj_t *mmobj = vma->vma_obj;
	KASSERT(mmobj != NULL);
	dbg(DBG_PRINT, "(GRADING3A)\n");
	if (mmobj->mmo_shadowed != NULL) {
		forwrite = ((cause & FAULT_WRITE) == FAULT_WRITE);
		dbg(DBG_PRINT, "(GRADING3A)\n");
	}
	else{
		forwrite = 0;
		dbg(DBG_PRINT, "(GRADING3D 1)\n");
	}
// assume lookup will fill the page
	//pframe_get(mmobj, ADDR_TO_PN(vaddr) - vma->vma_start + vma->vma_off, &pf); // alloc successfully, but can't fill
//	if (mmobj->mmo_ops->lookuppage(mmobj, ADDR_TO_PN(vaddr), forwrite, &pf))	{ // 0 for success
//	pframe_lookup(mmobj, ADDR_TO_PN(vaddr), forwrite, &pf);
	if (pframe_lookup(mmobj, ADDR_TO_PN(vaddr) - vma->vma_start + vma->vma_off, forwrite, &pf)){
		dbg(DBG_PRINT, "(GRADING3D 2)\n");
		proc_kill(curproc, EFAULT); // killing curproc doesn't return
	}

//	if (mmobj->mmo_ops->fillpage(mmobj, pf)) {
//	if (pframe_fill(pf)) {
//		proc_kill(curproc, -EFAULT); // killing curproc doesn't return
//	}

	// (let pf be the page frame located by the page fault handler)
	KASSERT(pf); /* this page frame must be non-NULL */
	KASSERT(pf->pf_addr); /* this page frame's pf_addr must be non-NULL */
	dbg(DBG_PRINT, "(GRADING3A 5.a)\n");
// update kernel - page table entry 
	KASSERT (0 == pt_map(curproc->p_pagedir, (uintptr_t)PAGE_ALIGN_DOWN(vaddr), pt_virt_to_phys((uintptr_t)pf->pf_addr), cause|PT_PRESENT, cause|PT_PRESENT));
	//not reached//if (0 != pt_map(curproc->p_pagedir, (uintptr_t)PAGE_ALIGN_DOWN(vaddr), pt_virt_to_phys((uintptr_t)pf->pf_addr), cause|PT_PRESENT, cause|PT_PRESENT)){
	//not reached//	dbg(DBG_PRINT, "(NEWPATH)\n");
	//not reached//	proc_kill(curproc, EFAULT); // killing curproc doesn't return
	//not reached//}
	dbg(DBG_PRINT, "(GRADING3A)\n");


//	if (mmobj->mmo_ops->fillpage(mmobj, pf)){ // 0 for success
//		proc_kill(curproc, -EFAULT); // killing curproc doesn't return
//	}

	
}
