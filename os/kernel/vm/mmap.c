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
#include "types.h"

#include "mm/mm.h"
#include "mm/tlb.h"
#include "mm/mman.h"
#include "mm/page.h"

#include "proc/proc.h"

#include "util/string.h"
#include "util/debug.h"

#include "fs/vnode.h"
#include "fs/vfs.h"
#include "fs/file.h"

#include "vm/vmmap.h"
#include "vm/mmap.h"

/*
 * This function implements the mmap(2) syscall, but only
 * supports the MAP_SHARED, MAP_PRIVATE, MAP_FIXED, and
 * MAP_ANON flags.
 *
 * Add a mapping to the current process's address space.
 * You need to do some error checking; see the ERRORS section
 * of the manpage for the problems you should anticipate.
 * After error checking most of the work of this function is
 * done by vmmap_map(), but remember to clear the TLB.
 */
int
do_mmap(void *addr, size_t len, int prot, int flags,
        int fd, off_t off, void **ret)
{
//        NOT_YET_IMPLEMENTED("VM: do_mmap");
//        return -1;
	   struct file * f = NULL;
		int result;
		vmarea_t *vma;

		if (!(flags &(MAP_SHARED | MAP_PRIVATE | MAP_FIXED | MAP_ANON))){
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
		    return -EINVAL;
		}
		if (addr == NULL && (flags & MAP_FIXED)){
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			return -EINVAL;
		}

		if (!PAGE_ALIGNED((size_t)addr)  
			|| (addr != NULL && (size_t)addr < USER_MEM_LOW) 
			|| ((size_t)addr + len > USER_MEM_HIGH) || (((size_t)addr + len) <= (size_t) addr) // len is unsigned
			){
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			return -EINVAL;
		}

		if (!(flags & MAP_ANON)){
			f = fget(fd);

		    if (f == NULL) {
		    	dbg(DBG_PRINT, "(GRADING3D 1)\n");
			    return -EBADF;
		    }

		    if ( //!( (f->f_vnode->vn_mode & 0xFF00) == 0x0800 ) ||
				(!((f->f_mode) & FMODE_READ) && (prot & PROT_READ)) || 
		    	((flags & MAP_SHARED) && (prot & PROT_WRITE) && (!((f->f_mode) & FMODE_READ) || !((f->f_mode) & FMODE_WRITE))) ||
		    	((prot & PROT_WRITE) &&  ((f->f_mode & (FMODE_APPEND|FMODE_READ|FMODE_WRITE)) == FMODE_APPEND  )) ) {
				fput(f);
			    dbg(DBG_PRINT, "(GRADING3D 1)\n");
				return -EACCES;
			}
			if (!PAGE_ALIGNED(off) || off < 0 ){
				fput(f);
				dbg(DBG_PRINT, "(GRADING3D 1)\n");
				return -EINVAL;
			}
			dbg(DBG_PRINT, "(GRADING3A)\n");

		}

        uint32_t lopage, npages;
		vmmap_t *map = curproc->p_vmmap;
		
		lopage = ADDR_TO_PN((size_t)addr);
		//npages =  PAGE_ALIGNED(len) ? ADDR_TO_PN(len) : ADDR_TO_PN(len) + 1;
		if (PAGE_ALIGNED(len)){
			npages = ADDR_TO_PN(len);
			dbg(DBG_PRINT, "(GRADING3A)\n");
		}
		else{
			npages = ADDR_TO_PN(len) + 1;
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
		}

		int dir = VMMAP_DIR_HILO;//TODO

//		tlb_flush_range((uintptr_t)addr, npages);
		tlb_flush_all();
		if (!(flags & MAP_ANON)){
			KASSERT(vmmap_map(map, f->f_vnode, lopage, npages, prot, flags, off / PAGE_SIZE, dir, &vma) == 0);
			//if ((result = vmmap_map(map, f->f_vnode, lopage, npages, prot, flags, off / PAGE_SIZE, dir, &vma)) != 0){ 
			//	fput(f);
			//	dbg(DBG_PRINT, "(NEWPATH)\n");
			//	return result;
			//}
			dbg(DBG_PRINT, "(GRADING3A)\n");
		}else{
			if ((result = vmmap_map(map, NULL, lopage, npages, prot, flags, 0, dir, &vma)) != 0){ 
				dbg(DBG_PRINT, "(GRADING3D 2)\n");
				return result;
			}
			dbg(DBG_PRINT, "(GRADING3D 2)\n");
		}
		
        
        //fput(f);
	    KASSERT(NULL != curproc->p_pagedir); //page table must be valid after a memory segment is mapped into the address space
        dbg(DBG_PRINT, "(GRADING3A 2.a)\n");
        //TODO: pagedir
		*ret = PN_TO_ADDR(vma->vma_start);
		if(!(flags & MAP_ANON)) {              
		   fput(f);
		    dbg(DBG_PRINT, "(GRADING3A)\n");           
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0;		  
}


/*
 * This function implements the munmap(2) syscall.
 *
 * As with do_mmap() it should perform the required error checking,
 * before calling upon vmmap_remove() to do most of the work.
 * Remember to clear the TLB.
 */
int
do_munmap(void *addr, size_t len)
{
//        NOT_YET_IMPLEMENTED("VM: do_munmap");
//        return -1;
		if (addr == NULL ){
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			return -EINVAL;
		}

		if(!PAGE_ALIGNED(addr)){
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			return -EINVAL;
		}
		int npages;

		if (!PAGE_ALIGNED((size_t)addr)  
			|| (addr != NULL && (size_t)addr < USER_MEM_LOW) 
			|| ((size_t)addr + len > USER_MEM_HIGH) || (((size_t)addr + len) <= (size_t)addr)){ // len is unsigned
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
			return -EINVAL;
		}

		//npages =  ADDR_TO_PN(PAGE_ALIGN_UP(len));
		//npages =  PAGE_ALIGNED(len) ? ADDR_TO_PN(len) : ADDR_TO_PN(len) + 1;
		if (PAGE_ALIGNED(len)){
			npages = ADDR_TO_PN(len);
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
		}
		else{
			npages = ADDR_TO_PN(len) + 1;
			dbg(DBG_PRINT, "(GRADING3D 1)\n");
		}
		// TODO: Which one is better?
		//tlb_flush_range((uintptr_t)addr, npages);
		tlb_flush_all();

		// TODO: Need to check return value?
		dbg(DBG_PRINT, "(GRADING3D 1)\n");
		return vmmap_remove(curproc->p_vmmap, ADDR_TO_PN(addr), npages);
}

