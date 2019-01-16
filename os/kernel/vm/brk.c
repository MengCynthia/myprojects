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

#include "globals.h"
#include "errno.h"
#include "util/debug.h"

#include "mm/mm.h"
#include "mm/page.h"
#include "mm/mman.h"

#include "vm/mmap.h"
#include "vm/vmmap.h"

#include "proc/proc.h"

/*
 * This function implements the brk(2) system call.
 *
 * This routine manages the calling process's "break" -- the ending address
 * of the process's "dynamic" region (often also referred to as the "heap").
 * The current value of a process's break is maintained in the 'p_brk' member
 * of the proc_t structure that represents the process in question.
 *
 * The 'p_brk' and 'p_start_brk' members of a proc_t struct are initialized
 * by the loader. 'p_start_brk' is subsequently never modified; it always
 * holds the initial value of the break. Note that the starting break is
 * not necessarily page aligned!
 *
 * 'p_start_brk' is the lower limit of 'p_brk' (that is, setting the break
 * to any value less than 'p_start_brk' should be disallowed).
 *
 * The upper limit of 'p_brk' is defined by the minimum of (1) the
 * starting address of the next occuring mapping or (2) USER_MEM_HIGH.
 * That is, growth of the process break is limited only in that it cannot
 * overlap with/expand into an existing mapping or beyond the region of
 * the address space allocated for use by userland. (note the presence of
 * the 'vmmap_is_range_empty' function).
 *
 * The dynamic region should always be represented by at most ONE vmarea.
 * Note that vmareas only have page granularity, you will need to take this
 * into account when deciding how to set the mappings if p_brk or p_start_brk
 * is not page aligned.
 *
 * You are guaranteed that the process data/bss region is non-empty.
 * That is, if the starting brk is not page-aligned, its page has
 * read/write permissions.
 *
 * If addr is NULL, you should "return" the current break. We use this to
 * implement sbrk(0) without writing a separate syscall. Look in
 * user/libc/syscall.c if you're curious.
 *
 * You should support combined use of brk and mmap in the same process.
 *
 * Note that this function "returns" the new break through the "ret" argument.
 * Return 0 on success, -errno on failure.
 */
int
do_brk(void *addr, void **ret)
{
//        NOT_YET_IMPLEMENTED("VM: do_brk");
//        return 0;
	uint32_t startvfn, endvfn, npages;
	vmarea_t *vma = NULL;
// 11/22 add addr!=null
	if (addr == NULL){
		*ret = curproc->p_brk;
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return 0;
	}

	if (((uint32_t) addr < (uint32_t)curproc->p_start_brk || (uint32_t) addr >= USER_MEM_HIGH ) && addr != NULL ){ // 11/22 add addr!=null
		dbg(DBG_PRINT, "(GRADING3D 1)\n");
		return -ENOMEM;
	}
	if (PAGE_ALIGNED(addr) ){
		endvfn = ADDR_TO_PN(addr);
		dbg(DBG_PRINT, "(GRADING3A)\n");
	}
	else{
		endvfn = ADDR_TO_PN(addr) + 1;
		dbg(DBG_PRINT, "(GRADING3D 1)\n");
	}
	// addr <= PAGE_ALIGN_UP(bss+data) && start_brk not aligned
	if ((endvfn-1) == ADDR_TO_PN(curproc->p_start_brk) && !PAGE_ALIGNED(curproc->p_start_brk)){ // endvfn is exclusive
		// must be sharing area with bss+data
		vma = vmmap_lookup(curproc->p_vmmap, ADDR_TO_PN(curproc->p_start_brk));
		KASSERT(vma != NULL);
		dbg(DBG_PRINT, "(GRADING3D 2)\n");
		if (vma->vma_end > ADDR_TO_PN(curproc->p_start_brk)+1){
			KASSERT(do_munmap( PAGE_ALIGN_UP(addr) , (size_t)PN_TO_ADDR(vma->vma_end) - (size_t)PAGE_ALIGN_UP(addr)) == 0) ; 
			//if (do_munmap( PAGE_ALIGN_UP(addr) , (size_t)PN_TO_ADDR(vma->vma_end) - (size_t)PAGE_ALIGN_UP(addr)) ){ //error
			//	*ret = NULL;
			//	dbg(DBG_PRINT, "(NEWPATH)\n");
			//	return -1; // if remove fail
			//}
			dbg(DBG_PRINT, "(GRADING3D 2)\n");
		}
		curproc->p_brk = addr;
		*ret = addr;
		dbg(DBG_PRINT, "(GRADING3D 2)\n");
		return 0;
	}
	//startvfn = PAGE_ALIGNED(curproc->p_start_brk) ? ADDR_TO_PN(curproc->p_start_brk) : ADDR_TO_PN(curproc->p_start_brk) + 1; 
	// not reached (should include)
	//if (PAGE_ALIGNED(curproc->p_start_brk)){
	// 	startvfn = ADDR_TO_PN(curproc->p_start_brk);
   //	dbg(DBG_PRINT, "(NEWPATH)\n");
	//}
	//else{
		startvfn = ADDR_TO_PN(curproc->p_start_brk) + 1;
		dbg(DBG_PRINT, "(GRADING3A)\n");
	//}
// if setting back to start_brk, remove dynamic region if there is any
/* can include but not reached
	if (addr == curproc->p_start_brk && addr != NULL){// 11/22 add addr!=null
		if ((vma = vmmap_lookup(curproc->p_vmmap, startvfn)) != NULL) {
//			p_start_brk may not be page aligned, but startvfn is
//startvfn, (ADDR_TO_PN(PAGE_ALIGN_UP(curproc->p_brk)) - startvfn)
//			if (vmmap_remove(curproc->p_vmmap, vma->vma_start, vma->vma_end ) != 0){ // assume exclusive
			if (do_munmap(PN_TO_ADDR((void*)vma->vma_start), (size_t)PN_TO_ADDR((void*)(vma->vma_end - vma->vma_start)))){
				*ret = NULL;
				dbg(DBG_PRINT, "(NEWPATH)\n");
				return -1; // if remove fail
			}
			dbg(DBG_PRINT, "(NEWPATH)\n");
		}
		curproc->p_brk = curproc->p_start_brk;
		*ret = curproc->p_brk;
		dbg(DBG_PRINT, "(NEWPATH)\n");
		return 0;
	}
*/	
	// if addr != curproc->p_start_brk
	// 1. if p_start_brk == p_brk (not created yet), we will build a new area. 
	//    if nothing betweeen p_start_brk to addr, call vmmap_map. if everything is ok, return
	//    otherwise, decrement endvfn
	// 2. if p_start_brk != p_brk (already exists), look up the area & change boundary (don't add/remove pframes for now)
	dbg(DBG_PRINT, "(GRADING3A)\n");
	if (curproc->p_start_brk == curproc->p_brk){
//		for (pn = endvfn; pn > startvfn; pn--){ // loop
			npages = endvfn - startvfn; // TODO: check inclusive or ex
			KASSERT(vmmap_is_range_empty(curproc->p_vmmap, startvfn, npages)); // must be empty since the following is commented out
			//if (vmmap_is_range_empty(curproc->p_vmmap, startvfn, npages)){ // use startvfn because start_brk may overlap with data+bss
				//dbg(DBG_PRINT, "(GRADING3A)\n");
				KASSERT(!PAGE_ALIGNED(curproc->p_start_brk));
				dbg(DBG_PRINT, "(GRADING3A)\n");
				//if (!PAGE_ALIGNED(curproc->p_start_brk)){ // means  data+bss shared with heap
					vma = vmmap_lookup(curproc->p_vmmap, ADDR_TO_PN(curproc->p_start_brk));
					KASSERT(vma != NULL); // must exist since data+bss is there
					dbg(DBG_PRINT, "(GRADING3A)\n");
					vma->vma_end = endvfn; //pn;
					//KASSERT(pn != endvfn);
					//dbg(DBG_PRINT, "(GRADING3A)\n");
					//if (pn == endvfn)
						*ret = addr;
						curproc->p_brk = addr;
						dbg(DBG_PRINT, "(GRADING3A)\n");
					return 0;
				// not needed//} // added
				//} // PAGE_ALIGNED(curproc->p_start_brk)

				/* page aligned case not reached
				if (vmmap_map(curproc->p_vmmap, NULL, startvfn, npages, PROT_WRITE | PROT_READ, MAP_PRIVATE, 0, VMMAP_DIR_LOHI, &vma) != 0){
					*ret = NULL;
					dbg(DBG_PRINT, "(NEWPATH)\n");
					return -1;
				}
				else{
						*ret = addr;
						curproc->p_brk = addr;
						dbg(DBG_PRINT, "(NEWPATH)\n");
					return 0;
				}
				*/
				//dbg(DBG_PRINT, "(GRADING3A)\n");
			//}
			// else overlap
			//else {
			//	*ret = NULL;
			//dbg(DBG_PRINT, "(NEWPATH)\n");
			//	return -1;
			//}
//		} // loop end
		//notreached//*ret = NULL;
		//notreached//dbg(DBG_PRINT, "(NEWPATH)\n");
		//notreached//return -ENOMEM;
	}
	else {
		// must exists an area
		vma = vmmap_lookup(curproc->p_vmmap, ADDR_TO_PN(curproc->p_start_brk));
		KASSERT(vma != NULL);
		dbg(DBG_PRINT, "(GRADING3A)\n");
		if (vma->vma_end > endvfn){ // TODO: check inclusive or ex
//			if (vmmap_remove(curproc->p_vmmap, endvfn, vma->vma_end - endvfn) != 0){ // assume endvfn exclusive
		   KASSERT (do_munmap(PN_TO_ADDR((void*)endvfn), (size_t)PN_TO_ADDR((void*)(vma->vma_end - endvfn))) == 0);
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			//if (do_munmap(PN_TO_ADDR((void*)endvfn), (size_t)PN_TO_ADDR((void*)(vma->vma_end - endvfn)))){
			//	*ret = NULL;
			//	dbg(DBG_PRINT, "(NEWPATH)\n");
			//	return -1;
			//}
			//else {
				*ret = addr;
				curproc->p_brk = addr;
				dbg(DBG_PRINT, "(GRADING3D 1)\n");
				return 0;
			//}
			//dbg(DBG_PRINT, "(NEWPATH)\n");
		}
		else if (vma->vma_end == endvfn){
			*ret = addr;
			curproc->p_brk = addr;
			dbg(DBG_PRINT, "(GRADING3D 2)\n");
			return 0;
		}
		else{
			//while(endvfn > vma->vma_end && !vmmap_is_range_empty(curproc->p_vmmap, vma->vma_end, endvfn - vma->vma_end)){
			//	endvfn--;
			//}
			// increase heap area
			if (!vmmap_is_range_empty(curproc->p_vmmap, vma->vma_end, endvfn - vma->vma_end)){
				*ret = NULL;
				dbg(DBG_PRINT, "(GRADING3D 2)\n");
				return -1;
			}
			vma->vma_end = endvfn; 
			*ret = addr;
			curproc->p_brk = addr;
			dbg(DBG_PRINT, "(GRADING3A)\n");
			return 0;
			//}
		}
		//dbg(DBG_PRINT, "(NEWPATH)\n");
	}
    //dbg(DBG_PRINT, "(NEWPATH)\n");
	return 0;
}
