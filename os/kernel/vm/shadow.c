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

#include "util/string.h"
#include "util/debug.h"

#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/mm.h"
#include "mm/page.h"
#include "mm/slab.h"
#include "mm/tlb.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/shadowd.h"

#define SHADOW_SINGLETON_THRESHOLD 5

int shadow_count = 0; /* for debugging/verification purposes */
#ifdef __SHADOWD__
/*
 * number of shadow objects with a single parent, that is another shadow
 * object in the shadow objects tree(singletons)
 */
static int shadow_singleton_count = 0;
#endif

static slab_allocator_t *shadow_allocator;

static void shadow_ref(mmobj_t *o);
static void shadow_put(mmobj_t *o);
static int  shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  shadow_fillpage(mmobj_t *o, pframe_t *pf);
static int  shadow_dirtypage(mmobj_t *o, pframe_t *pf);
static int  shadow_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t shadow_mmobj_ops = {
        .ref = shadow_ref,
        .put = shadow_put,
        .lookuppage = shadow_lookuppage,
        .fillpage  = shadow_fillpage,
        .dirtypage = shadow_dirtypage,
        .cleanpage = shadow_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * shadow page sub system. Currently it only initializes the
 * shadow_allocator object.
 */
void
shadow_init()
{
//        NOT_YET_IMPLEMENTED("VM: shadow_init");
        shadow_allocator = slab_allocator_create("shadow", sizeof(struct mmobj));
        /* after initialization, shadow_allocator must not be NULL */
        KASSERT(shadow_allocator); // (postcondition)
        dbg(DBG_PRINT, "(GRADING3A 6.a)\n");

}

/*
 * You'll want to use the shadow_allocator to allocate the mmobj to
 * return, then then initialize it. Take a look in mm/mmobj.h for
 * macros or functions which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
shadow_create()
{
//        NOT_YET_IMPLEMENTED("VM: shadow_create");
//        return NULL;
        mmobj_t *shadow_obj = (mmobj_t *)slab_obj_alloc(shadow_allocator);
        KASSERT(shadow_obj);
        dbg(DBG_PRINT, "(GRADING3A)\n");
        //mmobj_init(shadow_obj, &shadow_mmobj_ops);// can't use it for shadow
		  shadow_obj->mmo_ops = &shadow_mmobj_ops;
        //shadow_obj->mmo_refcount = 0;
        shadow_obj->mmo_nrespages = 0;
        list_init(&(shadow_obj->mmo_respages));
        //list_init(&shadow_obj->mmo_un.mmo_vmas);
        shadow_obj->mmo_shadowed = NULL;

        shadow_obj->mmo_refcount = 1; // TODO: CHECK
        shadow_obj->mmo_un.mmo_bottom_obj = NULL; // TODO: CHECK
        shadow_count++; // for debugging purpose
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return shadow_obj;
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
shadow_ref(mmobj_t *o)
{
//        NOT_YET_IMPLEMENTED("VM: shadow_ref");
        /* the o function argument must be non-NULL, has a positive refcount, and is a shadow object */
        KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops)); //(precondition) 
        dbg(DBG_PRINT, "(GRADING3A 6.b)\n");
        o->mmo_refcount++;
        dbg(DBG_PRINT, "(GRADING3A)\n");
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is a shadow object, it will never
 * be used again. You should unpin and uncache all of the object's
 * pages and then free the object itself.
 */
static void
shadow_put(mmobj_t *o)
{
//        NOT_YET_IMPLEMENTED("VM: shadow_put");
       /* the o function argument must be non-NULL, has a positive refcount, and is a shadow object */        
        KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops));//(precondition)
        dbg(DBG_PRINT, "(GRADING3A 6.c)\n");

        pframe_t *vp;
        if ((o->mmo_nrespages == (o->mmo_refcount-1))){
                list_iterate_begin(&o->mmo_respages, vp, pframe_t, pf_olink){

                        while (pframe_is_pinned(vp)){
                                pframe_unpin(vp); 
        								 dbg(DBG_PRINT, "(GRADING3A)\n");
                        }
								KASSERT(!pframe_is_busy(vp));
                        //not reached//while (pframe_is_busy(vp)){
                        //not reached//        sched_sleep_on(&(vp->pf_waitq));
        						//not reached//			dbg(DBG_PRINT, "(NEWPATH)\n");
                        //not reached//}
                        pframe_free(vp);
        						dbg(DBG_PRINT, "(GRADING3A)\n");
                } list_iterate_end();
        			dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        if(0 < --o->mmo_refcount){
        			dbg(DBG_PRINT, "(GRADING3A)\n");
                return;
        }

        slab_obj_free(shadow_allocator, o);
        shadow_count--;
        dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* This function looks up the given page in this shadow object. The
 * forwrite argument is true if the page is being looked up for
 * writing, false if it is being looked up for reading. This function
 * must handle all do-not-copy-on-not-write magic (i.e. when forwrite
 * is false find the first shadow object in the chain which has the
 * given page resident). copy-on-write magic (necessary when forwrite
 * is true) is handled in shadow_fillpage, not here. It is important to
 * use iteration rather than recursion here as a recursive implementation
 * can overflow the kernel stack when looking down a long shadow chain */
static int
shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{
//        NOT_YET_IMPLEMENTED("VM: shadow_lookuppage");
//        return 0;
       pframe_t *vp = NULL, *new=NULL;
		 int ret = 0;
        if(!forwrite){
                while(NULL != o){
						 //o->mmo_ops->lookup
						 dbg(DBG_PRINT, "(GRADING3A)\n");
						 if (o->mmo_shadowed != NULL){
							 /*
                        list_iterate_begin(&o->mmo_respages, vp, pframe_t, pf_olink){
                                if(pagenum == vp->pf_pagenum){
											  while (pframe_is_busy(vp)){
                              			sched_sleep_on(&vp->pf_waitq);
                        			  }
                                 	*pf = vp;
												// on return, (*pf) must be non-NULL //
                        				KASSERT(NULL != (*pf));//(postcondition when returning 0) 
                        				// on return, the page frame must have the right pagenum and it must not be in the "busy" state //
                        				KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf)));//(postcondition when returning 0)

												return 0; // found it & not busy
                                }
                        } list_iterate_end();
								*/
								while(1){
									vp = pframe_get_resident(o, pagenum);
									dbg(DBG_PRINT, "(GRADING3A)\n");
									if (vp != NULL){
										KASSERT(!pframe_is_busy(vp));
										dbg(DBG_PRINT, "(GRADING3A)\n");
										//not reached//if (pframe_is_busy(vp)){// the page is busy
										//not reached//	sched_sleep_on(&vp->pf_waitq);
        								//not reached//	dbg(DBG_PRINT, "(NEWPATH)\n");
				                  //not reached//  continue; //try again
                					//not reached//}
										//not reached//else{// found the page and not busy
                              	*pf = vp;
											// on return, (*pf) must be non-NULL //
                        			KASSERT(NULL != (*pf));//(postcondition when returning 0) 
                        			// on return, the page frame must have the right pagenum and it must not be in the "busy" state //
                        			KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf)));//(postcondition when returning 0)
        									dbg(DBG_PRINT, "(GRADING3A 6.d)\n");

                    					return 0;
										//not reached//}
        								//dbg(DBG_PRINT, "(NEWPATH)\n");
									}
									else {
        								dbg(DBG_PRINT, "(GRADING3A)\n");
										break;
										// break to next level
									}
        							//dbg(DBG_PRINT, "(NEWPATH)\n");
								}
        						dbg(DBG_PRINT, "(GRADING3A)\n");
						 }
						 else { // if o is not shadow obj
							 //if ((ret = o->mmo_ops->lookuppage(o, pagenum, forwrite, pf)) != 0){
							 //   *pf = NULL;
							 //   return ret;
							 //}
							 //vp = *pf;
        					 dbg(DBG_PRINT, "(GRADING3A)\n");
							 return o->mmo_ops->lookuppage(o, pagenum, forwrite, pf); // last level
						 }
                   //if(NULL != vp){
                   //     break;
                   //}
                   o = o->mmo_shadowed;
        				 dbg(DBG_PRINT, "(GRADING3A)\n");
                }
                //if (NULL != vp && pagenum == vp->pf_pagenum){ 
                //        while (pframe_is_busy(vp)){
                //              sched_sleep_on(&vp->pf_waitq);
                //        }
                //        *pf = vp;
                //        /* on return, (*pf) must be non-NULL */
                //        KASSERT(NULL != (*pf));//(postcondition when returning 0) 
                //        /* on return, the page frame must have the right pagenum and it must not be in the "busy" state */
                //        KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf)));//(postcondition when returning 0)
                //        return 0;
                //        
                //}else{
					 // not found
					//not reached//	 *pf = NULL;
        			//not reached//	 dbg(DBG_PRINT, "(NEWPATH)\n");
					//not reached//	 return -1;
                //}
        }else{
			  		if (pframe_get(o, pagenum, pf) != 0 ){
						*pf = NULL;
        				dbg(DBG_PRINT, "(GRADING3D 2)\n");
						return -1;
		  			}
                //new = pframe_alloc(o, pagenum);
					 //if (new == NULL){
					 //   *pf = NULL;
					 //   return -1;
                //}
                //if ( o->mmo_ops->fillpage(o, *pf) != 0 ){
					 //   *pf = NULL;
					 //   return -1;
					 //}
        			dbg(DBG_PRINT, "(GRADING3A)\n");
        }
			/* on return, (*pf) must be non-NULL */
         KASSERT(NULL != (*pf));//(postcondition when returning 0) 
         /* on return, the page frame must have the right pagenum and it must not be in the "busy" state */
         KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf)));//(postcondition when returning 0)
        dbg(DBG_PRINT, "(GRADING3A 6.d)\n");
        return 0;					 
}

/* As per the specification in mmobj.h, fill the page frame starting
 * at address pf->pf_addr with the contents of the page identified by
 * pf->pf_obj and pf->pf_pagenum. This function handles all
 * copy-on-write magic (i.e. if there is a shadow object which has
 * data for the pf->pf_pagenum-th page then we should take that data,
 * if no such shadow object exists we need to follow the chain of
 * shadow objects all the way to the bottom object and take the data
 * for the pf->pf_pagenum-th page from the last object in the chain).
 * It is important to use iteration rather than recursion here as a
 * recursive implementation can overflow the kernel stack when
 * looking down a long shadow chain */
static int
shadow_fillpage(mmobj_t *o, pframe_t *pf)
{
//        NOT_YET_IMPLEMENTED("VM: shadow_fillpage");
//        return 0;
	KASSERT(pframe_is_busy(pf)); /* can only "fill" a page frame when the page frame is in the "busy" state */
	KASSERT(!pframe_is_pinned(pf)); /* cannot pin a page frame that's already pinned */
   dbg(DBG_PRINT, "(GRADING3A 6.e)\n");
	pframe_t *orig = NULL;
	int ret = 0;
	if ( (ret = o->mmo_shadowed->mmo_ops->lookuppage(o->mmo_shadowed, pf->pf_pagenum, 0, &orig)) != 0 ){
      dbg(DBG_PRINT, "(GRADING3D 2)\n");
		return -1;
	}
	memcpy(pf->pf_addr, orig->pf_addr, PAGE_SIZE);
   dbg(DBG_PRINT, "(GRADING3A)\n");
	return 0;
}

/* These next two functions are not difficult. */

static int
shadow_dirtypage(mmobj_t *o, pframe_t *pf)
{
//        NOT_YET_IMPLEMENTED("VM: shadow_dirtypage");
//        return -1;
	pframe_pin(pf);
	pframe_set_dirty(pf);
   dbg(DBG_PRINT, "(GRADING3A)\n");
	return 0; 
}

static int
shadow_cleanpage(mmobj_t *o, pframe_t *pf)
{
        NOT_YET_IMPLEMENTED("VM: shadow_cleanpage");
        return -1;
//	pframe_unpin(pf);// TODO: do we need to keep unpining?
//	pframe_clear_dirty(pf);
//   dbg(DBG_TEST, "(NEWPATH)\n");
//	return 0;
}
