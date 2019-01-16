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
#include "errno.h"
#include "globals.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "proc/proc.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/vfs_syscall.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/mmobj.h"

static slab_allocator_t *vmmap_allocator;
static slab_allocator_t *vmarea_allocator;

void
vmmap_init(void)
{
        vmmap_allocator = slab_allocator_create("vmmap", sizeof(vmmap_t));
        KASSERT(NULL != vmmap_allocator && "failed to create vmmap allocator!");
        vmarea_allocator = slab_allocator_create("vmarea", sizeof(vmarea_t));
        KASSERT(NULL != vmarea_allocator && "failed to create vmarea allocator!");
}

vmarea_t *
vmarea_alloc(void)
{
        vmarea_t *newvma = (vmarea_t *) slab_obj_alloc(vmarea_allocator);
        if (newvma) {
                newvma->vma_vmmap = NULL;
        }
        return newvma;
}

void
vmarea_free(vmarea_t *vma)
{
        KASSERT(NULL != vma);
        slab_obj_free(vmarea_allocator, vma);
}

/* a debugging routine: dumps the mappings of the given address space. */
size_t
vmmap_mapping_info(const void *vmmap, char *buf, size_t osize)
{
        KASSERT(0 < osize);
        KASSERT(NULL != buf);
        KASSERT(NULL != vmmap);

        vmmap_t *map = (vmmap_t *)vmmap;
        vmarea_t *vma;
        ssize_t size = (ssize_t)osize;

        int len = snprintf(buf, size, "%21s %5s %7s %8s %10s %12s\n",
                           "VADDR RANGE", "PROT", "FLAGS", "MMOBJ", "OFFSET",
                           "VFN RANGE");

        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
                size -= len;
                buf += len;
                if (0 >= size) {
                        goto end;
                }

                len = snprintf(buf, size,
                               "%#.8x-%#.8x  %c%c%c  %7s 0x%p %#.5x %#.5x-%#.5x\n",
                               vma->vma_start << PAGE_SHIFT,
                               vma->vma_end << PAGE_SHIFT,
                               (vma->vma_prot & PROT_READ ? 'r' : '-'),
                               (vma->vma_prot & PROT_WRITE ? 'w' : '-'),
                               (vma->vma_prot & PROT_EXEC ? 'x' : '-'),
                               (vma->vma_flags & MAP_SHARED ? " SHARED" : "PRIVATE"),
                               vma->vma_obj, vma->vma_off, vma->vma_start, vma->vma_end);
        } list_iterate_end();

end:
        if (size <= 0) {
                size = osize;
                buf[osize - 1] = '\0';
        }
        /*
        KASSERT(0 <= size);
        if (0 == size) {
                size++;
                buf--;
                buf[0] = '\0';
        }
        */
        return osize - size;
}

/* Create a new vmmap, which has no vmareas and does
 * not refer to a process. */
vmmap_t *
vmmap_create(void)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_create");
//        return NULL;
        vmmap_t* vmmap = (vmmap_t *) slab_obj_alloc(vmmap_allocator);
        if (vmmap){
            list_init(&(vmmap -> vmm_list));
            vmmap -> vmm_proc = NULL;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return vmmap;
}
void putChainRefcount(mmobj_t *obj){
	mmobj_t *nextobj=NULL, *wobj=obj;
	while (wobj){
		if (wobj->mmo_shadowed != NULL) {// shadowed object
			nextobj = wobj->mmo_shadowed;
			if ((wobj->mmo_refcount - 1) == wobj->mmo_nrespages){
				wobj->mmo_ops->put(wobj);
				wobj = nextobj;
                dbg(DBG_PRINT, "(GRADING3A)\n");
			}
			else{
				wobj->mmo_ops->put(wobj);
                dbg(DBG_PRINT, "(GRADING3A)\n");
				break;
			}
            dbg(DBG_PRINT, "(GRADING3A)\n");
		}
		else{
			wobj->mmo_ops->put(wobj);
           dbg(DBG_PRINT, "(GRADING3A)\n");
			break;
		}
        dbg(DBG_PRINT, "(GRADING3A)\n");
	}
        dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* Removes all vmareas from the address space and frees the
 * vmmap struct. */
void
vmmap_destroy(vmmap_t *map)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_destroy");
        KASSERT(NULL != map); /* function argument must not be NULL */
        dbg(DBG_PRINT, "(GRADING3A 3.a)\n");
        vmarea_t *vma;
		  pframe_t *pf;
		  //mmobj_t *sobj=NULL, *bobj=NULL, *wobj=NULL;
        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
					 list_remove(&(vma->vma_olink));
					 putChainRefcount(vma->vma_obj);
					 /*
					 if (vma->vma_obj->mmo_shadowed != NULL) {// shadowed object
					 	sobj = vma->vma_obj->mmo_shadowed;
					 	bobj = vma->vma_obj->mmo_un.mmo_bottom_obj;
					 }
					 //list_iterate_begin(&(vma->vma_obj->mmo_respages), pf, pframe_t, pf_olink){
					 //  vma->vma_obj->mmo_ops->cleanpage(vma->vma_obj, pf);
					 //}list_iterate_end();
					 vma->vma_obj->mmo_ops->put(vma->vma_obj); // plink will be taken care by put 
					 if (vma->vma_obj->mmo_refcount == vma->vma_obj->mmo_nrespages){
						if (sobj != NULL){ // if it was a shadow obj
							KASSERT(bobj != NULL);
							sobj->mmo_ops->put(sobj);
//							bobj->mmo_ops->put(bobj);
						}
					 }
					 */
                vmarea_free(vma);
                dbg(DBG_PRINT, "(GRADING3A)\n");
					 
        } list_iterate_end();
        slab_obj_free(vmmap_allocator, map);
        dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* Add a vmarea to an address space. Assumes (i.e. asserts to some extent)
 * the vmarea is valid.  This involves finding where to put it in the list
 * of VM areas, and adding it. Don't forget to set the vma_vmmap for the
 * area. */
void
vmmap_insert(vmmap_t *map, vmarea_t *newvma)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_insert");
        KASSERT(NULL != map && NULL != newvma); /* both function arguments must not be NULL */
        KASSERT(NULL == newvma->vma_vmmap); /* newvma must be newly create and must not be part of any existing vmmap */
        KASSERT(newvma->vma_start < newvma->vma_end); /* newvma must not be empty */
        KASSERT(ADDR_TO_PN(USER_MEM_LOW) <= newvma->vma_start && ADDR_TO_PN(USER_MEM_HIGH) >= newvma->vma_end);
                                  /* addresses in this memory segment must lie completely within the user space */
        dbg(DBG_PRINT, "(GRADING3A 3.b)\n");
        vmarea_t *vma;
		  dbg(DBG_PRINT, "(GRADING3A)\n");

		  if (!list_empty(&map->vmm_list)) {
        		list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
        		        if(vma->vma_start > newvma->vma_start){
        		            list_insert_before(&vma->vma_plink, &newvma->vma_plink);
        						  newvma->vma_vmmap = map;
        		            dbg(DBG_PRINT, "(GRADING3A)\n");
        		            return;
        		        }
                         dbg(DBG_PRINT, "(GRADING3A)\n");

        		} list_iterate_end();
				KASSERT(newvma->vma_start >= vma->vma_end);
				//if (newvma->vma_start >= vma->vma_end){
					list_insert_tail(&map->vmm_list,  &newvma->vma_plink);
					newvma->vma_vmmap = map;
					dbg(DBG_PRINT, "(GRADING3A)\n");
					return;
				//}
            //    dbg(DBG_PRINT, "(NEWPATH)\n");
		  }
		  else{
			  list_insert_head(&map->vmm_list,  &newvma->vma_plink);
			  newvma->vma_vmmap = map;
               dbg(DBG_PRINT, "(GRADING3A)\n");
			  return;
		  }
        //   dbg(DBG_PRINT, "(NEWPATH)\n");
}

/* Find a contiguous range of free virtual pages of length npages in
 * the given address space. Returns starting vfn for the range,
 * without altering the map. Returns -1 if no such range exists.
 *
 * Your algorithm should be first fit. If dir is VMMAP_DIR_HILO, you
 * should find a gap as high in the address space as possible; if dir
 * is VMMAP_DIR_LOHI, the gap should be as low as possible. */
int
vmmap_find_range(vmmap_t *map, uint32_t npages, int dir)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_find_range");
//        return -1;
        vmarea_t *vma;
        //int vfn;
        //int gap;
        int afterstart = ADDR_TO_PN(USER_MEM_HIGH);
        int beforeend = ADDR_TO_PN(USER_MEM_LOW);
		  KASSERT(dir == VMMAP_DIR_HILO);
		  dbg(DBG_PRINT, "(GRADING3A)\n");
        //if (dir == VMMAP_DIR_HILO){
            list_iterate_reverse(&map->vmm_list, vma, vmarea_t, vma_plink) {
                    if (afterstart - vma->vma_end >= npages){
                        dbg(DBG_PRINT, "(GRADING3A)\n");
                        return afterstart - npages;
                    }else{
                        afterstart = vma->vma_start;
                       dbg(DBG_PRINT, "(GRADING3D 1)\n");
                    }
                    dbg(DBG_PRINT, "(GRADING3D 1)\n");
                    
            } list_iterate_end();
				if (afterstart - npages >= ADDR_TO_PN(USER_MEM_LOW)){
                    dbg(DBG_PRINT, "(GRADING3D 2)\n");
					return afterstart - npages;
				}
            //dbg(DBG_PRINT, "(GRADING3D 2)\n");

		  // not reached //}else{
        // not reached //    list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
        // not reached //            if (vma->vma_start - beforeend >= npages){
        // not reached //                dbg(DBG_PRINT, "(NEWPATH)\n");
        // not reached //                return beforeend;
        // not reached //            }else{
        // not reached //                beforeend = vma->vma_end;
        // not reached //                dbg(DBG_PRINT, "(NEWPATH)\n");
        // not reached //            }
        // not reached //            dbg(DBG_PRINT, "(NEWPATH)\n");
        // not reached //    } list_iterate_end();
		  // not reached // 	if (beforeend + npages <= ADDR_TO_PN(USER_MEM_HIGH)){
        // not reached //            dbg(DBG_PRINT, "(NEWPATH)\n");
		  // not reached // 		return beforeend;
		  // not reached // 	}
        // not reached //        dbg(DBG_PRINT, "(NEWPATH)\n");
        // not reached //}
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return -1;
}

/* Find the vm_area that vfn lies in. Simply scan the address space
 * looking for a vma whose range covers vfn. If the page is unmapped,
 * return NULL. */
vmarea_t *
vmmap_lookup(vmmap_t *map, uint32_t vfn)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_lookup");
//        return NULL;
		KASSERT(NULL != map); /* the first function argument must not be NULL */
        dbg(DBG_PRINT, "(GRADING3A 3.c)\n");
        vmarea_t *vma;
        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {

                if (vma->vma_start <= vfn && vma->vma_end > vfn){ //vfn lies in
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    return vma;
                }
                dbg(DBG_PRINT, "(GRADING3A)\n");
                
        } list_iterate_end();
        dbg(DBG_PRINT, "(GRADING3C 5)\n");
        return NULL; //if unmapped
}

/* Allocates a new vmmap containing a new vmarea for each area in the
 * given map. The areas should have no mmobjs set yet. Returns pointer
 * to the new vmmap on success, NULL on failure. This function is
 * called when implementing fork(2). */
vmmap_t *
vmmap_clone(vmmap_t *map)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_clone");
//        return NULL;
        vmmap_t * newmap;
        newmap = vmmap_create();
		  KASSERT(newmap != NULL);
        dbg(DBG_PRINT, "(GRADING3A)\n");

        //if (newmap == NULL){
        //    dbg(DBG_PRINT, "(NEWPATH)\n");
        //    return NULL;
        //}
        vmarea_t *oldvma;
        vmarea_t *newvma;
		  mmobj_t* oldobj = NULL;
        list_iterate_begin(&map->vmm_list, oldvma, vmarea_t, vma_plink) {
            newvma =  vmarea_alloc();
				KASSERT(newvma != NULL);
            dbg(DBG_PRINT, "(GRADING3A)\n");
            //if (newvma == NULL){
            //    dbg(DBG_PRINT, "(NEWPATH)\n");
            //    return NULL;
            //}
            newvma->vma_start = oldvma->vma_start;
            newvma->vma_end = oldvma->vma_end;
            newvma->vma_off = oldvma->vma_off;
            newvma->vma_prot = oldvma->vma_prot;
            newvma->vma_flags = oldvma->vma_flags;
            newvma->vma_vmmap = newmap;
//            newvma->vma_obj = NULL;
            list_insert_tail(&(newmap->vmm_list),&(newvma->vma_plink));
            dbg(DBG_PRINT, "(GRADING3A)\n");
            if (newvma->vma_flags & MAP_PRIVATE){
                oldobj = oldvma->vma_obj;
                newvma->vma_obj = shadow_create(); //creat shadow obj for new area
					 KASSERT(newvma->vma_obj != NULL);
					 KASSERT(oldobj->mmo_shadowed != NULL); // must be shadow obj
                     dbg(DBG_PRINT, "(GRADING3A)\n");
					 //if (oldobj->mmo_shadowed != NULL){ // if old obj is also a shadow obj
                	newvma->vma_obj->mmo_un.mmo_bottom_obj = oldobj->mmo_un.mmo_bottom_obj;
//					 	oldobj->mmo_un.mmo_bottom_obj->mmo_ops->ref(oldobj->mmo_un.mmo_bottom_obj); 
					 	dbg(DBG_PRINT, "(GRADING3A)\n");
					 // not reached //}
					 // not reached //else { // if old obj is other type of mmobj
					 // not reached //   newvma->vma_obj->mmo_un.mmo_bottom_obj = oldobj;
//					 // not reached //   oldobj->mmo_ops->ref(oldobj);
					 // not reached //	dbg(DBG_PRINT, "(NEWPATH)\n");
					 // not reached //}
                newvma->vma_obj->mmo_shadowed = oldobj;
                oldobj->mmo_ops->ref(oldobj);
                mmobj_t *pnewobj = shadow_create(); // for existing map
					 KASSERT(pnewobj != NULL);
						KASSERT(oldobj->mmo_shadowed != NULL); // must be shadow obj
                  dbg(DBG_PRINT, "(GRADING3A)\n");
                //if (oldobj->mmo_shadowed != NULL){ // if old obj is also a shadow obj
					 	pnewobj->mmo_un.mmo_bottom_obj = oldobj->mmo_un.mmo_bottom_obj;
//					 	oldobj->mmo_un.mmo_bottom_obj->mmo_ops->ref(oldobj->mmo_un.mmo_bottom_obj);
                dbg(DBG_PRINT, "(GRADING3A)\n");
                // not reached //}
					 // not reached //else{ // if old obj is other type of mmobj
					 // not reached //  pnewobj->mmo_un.mmo_bottom_obj = oldobj;
//					 // not reached //  oldobj->mmo_ops->ref(oldobj);
					 // not reached //dbg(DBG_PRINT, "(NEWPATH)\n");
					 // not reached //}
					 pnewobj->mmo_shadowed = oldobj;
                oldvma->vma_obj = pnewobj; // insert a new shadow for old vma
					 // private must have a shadow at the 1st level
            	 list_insert_tail(&(oldvma->vma_obj->mmo_un.mmo_bottom_obj->mmo_un.mmo_vmas),&(newvma->vma_olink));
               dbg(DBG_PRINT, "(GRADING3A)\n");
            }else if (newvma->vma_flags & MAP_SHARED){
                newvma->vma_obj = oldvma->vma_obj;
                newvma->vma_obj->mmo_ops->ref(oldvma->vma_obj);
                list_insert_tail(&(oldvma->vma_obj->mmo_un.mmo_vmas),&(newvma->vma_olink));
                dbg(DBG_PRINT, "(GRADING3D 1)\n");
            }
              dbg(DBG_PRINT, "(GRADING3A)\n");
                
        } list_iterate_end();
       dbg(DBG_PRINT, "(GRADING3A)\n");
        return newmap;
}


/* Insert a mapping into the map starting at lopage for npages pages.
 * If lopage is zero, we will find a range of virtual addresses in the
 * process that is big enough, by using vmmap_find_range with the same
 * dir argument.  If lopage is non-zero and the specified region
 * contains another mapping that mapping should be unmapped.
 *
 * If file is NULL an anon mmobj will be used to create a mapping
 * of 0's.  If file is non-null that vnode's file will be mapped in
 * for the given range.  Use the vnode's mmap operation to get the
 * mmobj for the file; do not assume it is file->vn_obj. Make sure all
 * of the area's fields except for vma_obj have been set before
 * calling mmap.
 *
 * If MAP_PRIVATE is specified set up a shadow object for the mmobj.
 *
 * All of the input to this function should be valid (KASSERT!).
 * See mmap(2) for for description of legal input.
 * Note that off should be page aligned.
 *
 * Be very careful about the order operations are performed in here. Some
 * operation are impossible to undo and should be saved until there
 * is no chance of failure.
 *
 * If 'new' is non-NULL a pointer to the new vmarea_t should be stored in it.
 */
int
vmmap_map(vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
          int prot, int flags, off_t off, int dir, vmarea_t **new)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_map");
//        return -1;
       /* function arguments (except first and last) describe the new memory segment to be created and added to the address space */
        /* must not add a memory segment into a non-existing vmmap */
        KASSERT(NULL != map); //(precondition) 
        /* number of pages of this memory segment cannot be 0 */
        KASSERT(0 < npages); //(precondition)
        /* must specify whether the memory segment is shared or private */
        KASSERT((MAP_SHARED & flags) || (MAP_PRIVATE & flags)); //(precondition)
        /* if lopage is not zero, it must be a user space vpn */
        KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_LOW) <= lopage)); //(precondition)
        /* if lopage is not zero, the specified page range must lie completely within the user space */
        KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages))); //(precondition)
        /* the off argument must be page aligned */                   
        KASSERT(PAGE_ALIGNED(off)); //(precondition)
        dbg(DBG_PRINT, "(GRADING3A 3.d)\n");

        vmarea_t *vma = vmarea_alloc();
		  mmobj_t *oldobj = NULL;
		  KASSERT(vma != NULL);
          dbg(DBG_PRINT, "(GRADING3A)\n");
        int range;
        int ret;
        if(lopage == 0){
            if ((range = vmmap_find_range(map, npages, dir)) < 0){
                dbg(DBG_PRINT, "(GRADING3D 2)\n");
					return range;
				}
            vma->vma_start = range;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else if(lopage != 0){
            if(!vmmap_is_range_empty(map, lopage, npages)){
					KASSERT((ret = vmmap_remove(map, lopage, npages)) == 0);
                //not reached//if ((ret = vmmap_remove(map, lopage, npages)) < 0){
                //not reached//    dbg(DBG_PRINT, "(NEWPATH)\n");
					 //not reached//   return ret;
					 //not reached//}
                  dbg(DBG_PRINT, "(GRADING3A)\n");
                
            }
				vma->vma_start = lopage;
                dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        vma->vma_end = vma->vma_start + npages;
        vma->vma_prot = prot;
        vma->vma_off = off;
        vma->vma_flags = flags;
		  vma->vma_obj = NULL;
        //list_link_init(&vma->vma_plink);
        //list_link_init(&vma->vma_olink);
		  //vma->vma_vmmap = map; // will be taken care by vmmap_insert
        /* TODO: Need to be initialize?
        struct vmmap  *vma_vmmap; 
        struct mmobj  *vma_obj;    
        */
         if(file == NULL){
             vma->vma_obj = anon_create();
             dbg(DBG_PRINT, "(GRADING3A)\n");
         }else{
             ret = file->vn_ops->mmap(file, vma, &(vma->vma_obj));
				 KASSERT(vma->vma_obj != NULL);
				 //if (vma->vma_obj == NULL){
             //       dbg(DBG_PRINT, "(NEWPATH)\n");
				 //  	return ret;
				 //}
				 //vma->vma_obj->mmo_ops->ref(vma->vma_obj);
				 //if (vma->vma_obj != NULL){
				 //   vma->vma_obj->mmo_ops->ref(vma->vma_obj);
				 //}
				 //else{
				 //   return ret;
				 //}
                 dbg(DBG_PRINT, "(GRADING3A)\n");
			}
			// definitely not a shadow obj yet
			list_insert_tail(&(vma->vma_obj->mmo_un.mmo_vmas), &vma->vma_olink);
		  KASSERT(vma->vma_obj != NULL);
          dbg(DBG_PRINT, "(GRADING3A)\n");
			
        if((MAP_TYPE & flags) & MAP_PRIVATE){
            //if(file == NULL){
                //vma->vma_obj = shadow_create();
				oldobj = vma->vma_obj;
            vma->vma_obj = shadow_create();
				KASSERT(vma->vma_obj != NULL);
				KASSERT(oldobj->mmo_shadowed == NULL); // must not be shadow
               dbg(DBG_PRINT, "(GRADING3A)\n");
				//not reached//if (oldobj->mmo_shadowed != NULL){ // if old obj is shadow
				//not reached//	vma->vma_obj->mmo_un.mmo_bottom_obj = oldobj->mmo_un.mmo_bottom_obj; 
//				//not reached//	oldobj->mmo_un.mmo_bottom_obj->mmo_ops->ref(oldobj->mmo_un.mmo_bottom_obj);
				//not reached//dbg(DBG_PRINT, "(NEWPATH)\n");
				//not reached//}
				//not reached//else { // not shadow
					vma->vma_obj->mmo_un.mmo_bottom_obj = oldobj;
//					oldobj->mmo_ops->ref(oldobj);
				dbg(DBG_PRINT, "(GRADING3A)\n");
				//not reached//}
				vma->vma_obj->mmo_shadowed = oldobj;
//				oldobj->mmo_ops->ref(oldobj); // already inc
           dbg(DBG_PRINT, "(GRADING3A)\n");
        }

		  //KASSERT(vma->vma_obj != NULL);

        if(new != NULL){
            *new = vma;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        vmmap_insert(map, vma);
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0;
}

/*
 * We have no guarantee that the region of the address space being
 * unmapped will play nicely with our list of vmareas.
 *
 * You must iterate over each vmarea that is partially or wholly covered
 * by the address range [addr ... addr+len). The vm-area will fall into one
 * of four cases, as illustrated below:
 *
 * key:
 *          [             ]   Existing VM Area
 *        *******             Region to be unmapped
 *
 * Case 1:  [   ******    ]
 * The region to be unmapped lies completely inside the vmarea. We need to
 * split the old vmarea into two vmareas. be sure to increment the
 * reference count to the file associated with the vmarea.
 *
 * Case 2:  [      *******]**
 * The region overlaps the end of the vmarea. Just shorten the length of
 * the mapping.
 *
 * Case 3: *[*****        ]
 * The region overlaps the beginning of the vmarea. Move the beginning of
 * the mapping (remember to update vma_off), and shorten its length.
 *
 * Case 4: *[*************]**
 * The region completely contains the vmarea. Remove the vmarea from the
 * list.
 */
int
vmmap_remove(vmmap_t *map, uint32_t lopage, uint32_t npages)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_remove");
       vmarea_t *vma, *vmaNew; 
		 //mmobj_t *sobj=NULL, *bobj=NULL;
		 uint32_t tempEnd;
        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink){
            if(vma->vma_start < lopage && vma->vma_end > (lopage + npages)){
					 tempEnd = vma->vma_end;
                vma->vma_end = lopage;
                vmaNew = vmarea_alloc();
					 KASSERT(vmaNew != NULL);
                     dbg(DBG_PRINT, "(GRADING3D 2)\n");

                vmaNew->vma_start = lopage + npages;
                vmaNew->vma_end = tempEnd;
                vmaNew->vma_prot = vma->vma_prot;
                vmaNew->vma_flags = vma->vma_flags;
                vmaNew->vma_obj = vma->vma_obj;
                vmaNew->vma_off = (vma->vma_end - vma->vma_start + npages) + vma->vma_off; // TODO: check
                //list_link_init(&vmaNew->vmaNew_plink);
                //list_link_init(&vmaNew->vmaNew_olink);
					 //vmaNew->vma_vmmap = map; // taken care by insert
					 vmmap_insert(map, vmaNew);
					 KASSERT(vma->vma_obj->mmo_shadowed != NULL); // must be shadow
                     dbg(DBG_PRINT, "(GRADING3D 2)\n");
					 //if (vma->vma_obj->mmo_shadowed == NULL){ // not shadow
					 //	list_insert_tail(&(vma->vma_obj->mmo_un.mmo_vmas), &vmaNew->vma_olink); //added 
					 //  dbg(DBG_PRINT, "(NEWPATH)\n");
					 //}
					 //else{
						 //vmaNew->vma_obj->mmo_un.mmo_bottom_obj = vma->vma_obj->mmo_un.mmo_bottom_obj;
						list_insert_tail(&(vma->vma_obj->mmo_un.mmo_bottom_obj->mmo_un.mmo_vmas), &vmaNew->vma_olink);
					   dbg(DBG_PRINT, "(GRADING3D 2)\n");
					 //not reached//}
                pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(lopage), (uintptr_t)PN_TO_ADDR(lopage+npages));
                vma->vma_obj->mmo_ops->ref(vma->vma_obj); // added
                dbg(DBG_PRINT, "(GRADING3D 2)\n");
            }else if(vma->vma_start < lopage && lopage < vma->vma_end && vma->vma_end <= (lopage + npages) ){
                pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(lopage), (uintptr_t)PN_TO_ADDR(vma->vma_end));
                vma->vma_end = lopage;
               dbg(DBG_PRINT, "(GRADING3D 1)\n");
            }else if(vma->vma_start >= lopage && vma->vma_end > (lopage + npages) && vma->vma_start < (lopage + npages)){
                pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(vma->vma_start), (uintptr_t)PN_TO_ADDR(lopage+npages));
					 vma->vma_off += (lopage + npages - vma->vma_start);
                vma->vma_start = lopage + npages;
               dbg(DBG_PRINT, "(GRADING3D 2)\n");
            }else if(vma->vma_start >= lopage && vma->vma_end <= (lopage + npages)){
					 list_remove(&(vma->vma_olink));
					 putChainRefcount(vma->vma_obj);
					 /*
					 if (vma->vma_obj->mmo_shadowed != NULL) {// shadowed object
					 	sobj = vma->vma_obj->mmo_shadowed;
					 	bobj = vma->vma_obj->mmo_un.mmo_bottom_obj;
					 }

					 vma->vma_obj->mmo_ops->put(vma->vma_obj); // plink will be taken care by put
					 //if (vma->vma_obj == NULL){
					 if (vma->vma_obj->mmo_refcount == vma->vma_obj->mmo_nrespages){
						if (sobj != NULL){ // if it was a shadow obj
							KASSERT(bobj != NULL);
							sobj->mmo_ops->put(sobj);
//							bobj->mmo_ops->put(bobj);
						}
					 }
					 */
					 list_remove(&(vma->vma_plink));
                pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(vma->vma_start), (uintptr_t)PN_TO_ADDR(vma->vma_end));
                vmarea_free(vma);
                dbg(DBG_PRINT, "(GRADING3A)\n");
            }
            dbg(DBG_PRINT, "(GRADING3A)\n");

        }list_iterate_end();
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0; 

}

/*
 * Returns 1 if the given address space has no mappings for the
 * given range, 0 otherwise.
 */
int
vmmap_is_range_empty(vmmap_t *map, uint32_t startvfn, uint32_t npages)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_is_range_empty");
//        return 0;
        uint32_t endvfn = startvfn + npages;
        vmarea_t *vma;

        /* the specified page range must not be empty and lie completely within the user space */
        //(precondition)
        KASSERT((startvfn < endvfn) && (ADDR_TO_PN(USER_MEM_LOW) <= startvfn) && (ADDR_TO_PN(USER_MEM_HIGH) >= endvfn));
        dbg(DBG_PRINT, "(GRADING3A 3.e)\n");
        
        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink){
            if (vma->vma_start < endvfn && vma->vma_end > startvfn){ 
                dbg(DBG_PRINT, "(GRADING3A)\n");
                return 0;
            }
            dbg(DBG_PRINT, "(GRADING3A)\n");
         }list_iterate_end();
         dbg(DBG_PRINT, "(GRADING3A)\n");

        return 1;
}

/* Read into 'buf' from the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do so, you will want to find the vmareas
 * to read from, then find the pframes within those vmareas corresponding
 * to the virtual addresses you want to read, and then read from the
 * physical memory that pframe points to. You should not check permissions
 * of the areas. Assume (KASSERT) that all the areas you are accessing exist.
 * Returns 0 on success, -errno on error.
 */
int
vmmap_read(vmmap_t *map, const void *vaddr, void *buf, size_t count)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_read");
//        return 0;
       

        //TODO: Assume (KASSERT) that all the areas you are accessing exist.

        vmarea_t *vma;
        int vfnstart = ADDR_TO_PN(vaddr);
		  KASSERT((size_t)vaddr + count < USER_MEM_HIGH);
          dbg(DBG_PRINT, "(GRADING3A)\n");
        int vfnend = ADDR_TO_PN((size_t)vaddr + count);
        int i, pagenum, res;
		  struct pframe *pf;
        size_t copyed = 0, offset = 0;
        for (i = vfnstart; i <= vfnend; i++){ 
            vma = vmmap_lookup(map, ADDR_TO_PN(vaddr));
				KASSERT(vma != NULL); // added
				dbg(DBG_PRINT, "(GRADING3A)\n");
				//if (vma == NULL){
            //        dbg(DBG_PRINT, "(NEWPATH)\n");
				//	return -1;
				//}
            offset = 0;
            if (i == vfnstart){ // changed from 0
                //offset = (size_t)vaddr - (size_t)PN_TO_ADDR(vma->vma_start); // changed from vma 
					 offset = PAGE_OFFSET(vaddr);
                     dbg(DBG_PRINT, "(GRADING3A)\n");
            }
            
            pagenum = i - vma->vma_start + vma->vma_off;
            KASSERT((res = vma->vma_obj->mmo_ops->lookuppage(vma->vma_obj, pagenum, 0, &pf)) == 0); // update field names
               dbg(DBG_PRINT, "(GRADING3A)\n");
            //not reached//if (res != 0){
            //not reached//    dbg(DBG_PRINT, "(NEWPATH)\n");
            //not reached//    return res;
            //not reached//}else{
                if (i == vfnend){
                    //KASSERT(count > copyed); // what is this for?
                    memcpy((void*)((size_t)buf+copyed), (void*)((size_t)pf->pf_addr+offset),  count - copyed);
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                }else{
                    memcpy((void*)((size_t)buf+copyed), (void*)((size_t)pf->pf_addr+offset), PAGE_SIZE - offset);
                    copyed += PAGE_SIZE - offset;
                    dbg(DBG_PRINT, "(GRADING3D 1)\n");
                }
            //not reached//}
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0;
		  
}

/* Write from 'buf' into the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do this, you will need to find the correct
 * vmareas to write into, then find the correct pframes within those vmareas,
 * and finally write into the physical addresses that those pframes correspond
 * to. You should not check permissions of the areas you use. Assume (KASSERT)
 * that all the areas you are accessing exist. Remember to dirty pages!
 * Returns 0 on success, -errno on error.
 */
int
vmmap_write(vmmap_t *map, void *vaddr, const void *buf, size_t count)
{
//        NOT_YET_IMPLEMENTED("VM: vmmap_write");
//        return 0;
        vmarea_t *vma=NULL;
        int vfnstart = ADDR_TO_PN(vaddr);
		  KASSERT((size_t)vaddr + count < USER_MEM_HIGH);
          dbg(DBG_PRINT, "(GRADING3A)\n");
        int vfnend = ADDR_TO_PN((size_t)vaddr + count);
        int i, pagenum, res;
		  struct pframe *pf;
        size_t copyed = 0, offset = 0;
        for (i = vfnstart; i <= vfnend; i++){
            vma = vmmap_lookup(map, ADDR_TO_PN(vaddr));
				KASSERT(vma != NULL); // added
          dbg(DBG_PRINT, "(GRADING3A)\n");
				//if (vma == NULL){
            //        dbg(DBG_PRINT, "(NEWPATH)\n");
				//	return -1;
				//}
            offset = 0;
            if (i == vfnstart){
                //offset = (size_t)vaddr - (size_t)PN_TO_ADDR(vma->vma_start); // changed from vma 
					 offset = PAGE_OFFSET(vaddr);
                dbg(DBG_PRINT, "(GRADING3A)\n");
            }
            
            pagenum = i - vma->vma_start + vma->vma_off;
            KASSERT((res = vma->vma_obj->mmo_ops->lookuppage(vma->vma_obj, pagenum, 1, &pf)) == 0); // update field names
                dbg(DBG_PRINT, "(GRADING3A)\n");
            //not reached//if (res != 0){
            //not reached//    dbg(DBG_PRINT, "(NEWPATH)\n");
            //not reached//    return res;
            //not reached//}else{
                if (i == vfnend){
                    //KASSERT(count > copyed);
						  KASSERT(vma->vma_obj->mmo_ops->dirtypage(vma->vma_obj, pf) == 0);
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    memcpy((void*)((size_t)pf->pf_addr+offset), (void*)((size_t)buf+copyed),  count - copyed);
                    dbg(DBG_PRINT, "(GRADING3A)\n");

                }else{
						  KASSERT(vma->vma_obj->mmo_ops->dirtypage(vma->vma_obj, pf) == 0);
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    memcpy((void*)((size_t)pf->pf_addr+offset), (void*)((size_t)buf+copyed), PAGE_SIZE - offset);
                    copyed += PAGE_SIZE - offset;
                   dbg(DBG_PRINT, "(GRADING3A)\n");
                }
            //not reached//}
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
/*
				if (vma == NULL){
					vma = vmarea_alloc();
					vma->vma_start = i;
               vma->vma_end = o
               vma->vma_off = o
               vma->vma_prot = 
               vma->vma_flags =
               vma->vma_vmmap =
               vma->vma_obj = N	
				}
				*/
       dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0;

}
