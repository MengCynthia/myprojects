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

int anon_count = 0; /* for debugging/verification purposes */

static slab_allocator_t *anon_allocator;

static void anon_ref(mmobj_t *o);
static void anon_put(mmobj_t *o);
static int  anon_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  anon_fillpage(mmobj_t *o, pframe_t *pf);
static int  anon_dirtypage(mmobj_t *o, pframe_t *pf);
static int  anon_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t anon_mmobj_ops = {
        .ref = anon_ref,
        .put = anon_put,
        .lookuppage = anon_lookuppage,
        .fillpage  = anon_fillpage,
        .dirtypage = anon_dirtypage,
        .cleanpage = anon_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * anonymous page sub system. Currently it only initializes the
 * anon_allocator object.
 */
void
anon_init()
{
//        NOT_YET_IMPLEMENTED("VM: anon_init");
	anon_allocator = slab_allocator_create("anon", sizeof(struct mmobj));
	KASSERT(anon_allocator); /* after initialization, anon_allocator must not be NULL */
	dbg(DBG_PRINT, "(GRADING3A 4.a)\n");
}

/*
 * You'll want to use the anon_allocator to allocate the mmobj to
 * return, then initialize it. Take a look in mm/mmobj.h for
 * definitions which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
anon_create()
{
//        NOT_YET_IMPLEMENTED("VM: anon_create");
//        return NULL;
	mmobj_t *anon_obj = (mmobj_t *)slab_obj_alloc(anon_allocator);
	KASSERT(anon_obj);
	dbg(DBG_PRINT, "(GRADING3A)\n");
	mmobj_init(anon_obj, &anon_mmobj_ops);
	anon_obj->mmo_refcount = 1; // TODO: CHECK
	anon_count++; // for debugging purpose
	dbg(DBG_PRINT, "(GRADING3A)\n");
	//	anon_ref(anon_obj);
	//anon_obj->mmo_refcount = 1; // Done by mmobj_init
	//anon_obj->mmo_nrespages = 0;
	//list_init(anon_obj->mmo_respages);
	//list_init(anon_obj->mmo_un.mmo_vmas);
	//anon_obj->mmo_shadowed = NULL;
	return anon_obj;
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
anon_ref(mmobj_t *o)
{
//        NOT_YET_IMPLEMENTED("VM: anon_ref");
   /* the o function argument must be non-NULL, has a positive refcount, and is an anonymous object */
	KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT, "(GRADING3A 4.b)\n");
	// TODO: CHECK
	o->mmo_refcount++;
	dbg(DBG_PRINT, "(GRADING3A)\n");
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is an anonymous object, it will
 * never be used again. You should unpin and uncache all of the
 * object's pages and then free the object itself.
 */
static void
anon_put(mmobj_t *o)
{
//        NOT_YET_IMPLEMENTED("VM: anon_put");
   /* the o function argument must be non-NULL, has a positive refcount, and is an anonymous object */
	KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT, "(GRADING3A 4.c)\n");
	pframe_t *vp;
	if ((o->mmo_nrespages == (o->mmo_refcount-1))){
		list_iterate_begin(&o->mmo_respages, vp, pframe_t, pf_olink) {
		        /*  (dbounov):
		         * Wait for the page to become not busy.
		         * At this point the only people who can be accessing the
		         * page are pframe_sync and the pageoutd (the shadowd does
		         * not touch non-anonymous objects). Both of them should
		         * definately free the page, if they have it busy.
		         */

				  // TODO: CHECK
				  // anon obj shouldn't be shared, so should just unpin once
					KASSERT(!pframe_is_pinned(vp));
				  //while (pframe_is_pinned(vp)){
				  //   pframe_unpin(vp); 
				  //   dbg(DBG_PRINT, "(NEWPATH)\n");
				  //}
				  KASSERT(!pframe_is_busy(vp));
		        dbg(DBG_PRINT, "(GRADING3A)\n");
		        //while (pframe_is_busy(vp)){
		        //        sched_sleep_on(&(vp->pf_waitq));
		        //        dbg(DBG_PRINT, "(NEWPATH)\n");
				  //}
		        pframe_free(vp);
		        dbg(DBG_PRINT, "(GRADING3A)\n");
		} list_iterate_end();
		//list_remove?
		dbg(DBG_PRINT, "(GRADING3A)\n");
	}
	if(0 < --o->mmo_refcount){
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return;
	}

		slab_obj_free(anon_allocator, o);
		anon_count--;
        dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* Get the corresponding page from the mmobj. No special handling is
 * required. */
static int
anon_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{
//        NOT_YET_IMPLEMENTED("VM: anon_lookuppage");
//        return -1;
// TODO : CHECK with vn_mmobj_ops
	dbg(DBG_PRINT, "(GRADING3A)\n");
	return pframe_get(o, pagenum, pf);
}

/* The following three functions should not be difficult. */

static int
anon_fillpage(mmobj_t *o, pframe_t *pf)
{
//        NOT_YET_IMPLEMENTED("VM: anon_fillpage");
//        return 0;
        /* can only "fill" a page frame when the page frame is in the "busy" state */
        KASSERT(pframe_is_busy(pf)); //(precondition)

        /* cannot pin a page frame that's already pinned */
        KASSERT(!pframe_is_pinned(pf)); //(precondition)
        dbg(DBG_PRINT, "(GRADING3A 4.d)\n");

        memset(pf->pf_addr, 0, PAGE_SIZE);

//        pframe_pin(pf);
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return 0;		  
}

static int
anon_dirtypage(mmobj_t *o, pframe_t *pf)
{
        NOT_YET_IMPLEMENTED("VM: anon_dirtypage");
        return -1;
// pin?
//	if  pframe_get(o, pf)
//	pframe_pin(pf);
//	pframe_set_dirty(pf);
//	dbg(DBG_TEST, "(NEWPATH)\n");
//	return 0;
}

static int
anon_cleanpage(mmobj_t *o, pframe_t *pf)
{
        NOT_YET_IMPLEMENTED("VM: anon_cleanpage");
        return -1;
//	pframe_unpin(pf); // TODO: do we need to keep unpining?
//	pframe_clear_dirty(pf);
//	dbg(DBG_TEST, "(NEWPATH)\n");
//	return 0;
}
