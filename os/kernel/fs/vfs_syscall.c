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

/*
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.2 2018/05/27 03:57:26 cvsps Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/*
 * Syscalls for vfs. Refer to comments or man pages for implementation.
 * Do note that you don't need to set errno, you should just return the
 * negative error code.
 */

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read vn_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */
int
do_read(int fd, void *buf, size_t nbytes)
{
//        NOT_YET_IMPLEMENTED("VFS: do_read");
//        return -1;
    struct file * f = NULL;
    struct vnode * vNode = NULL;
    int numOfReadBits = 0; 

	// step 1
	f = fget(fd);
	if (f == NULL) {
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	
	// fd is not open for reading 
	if (!((f->f_mode) & FMODE_READ)) {
		fput(f);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	// get the vnode for this file object
	vNode = f->f_vnode;

	// check fd refers to a directory
	if (S_ISDIR(vNode->vn_mode)) {
		fput(f);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EISDIR;
	}

	// step 2
	// int (*read)(struct vnode *file, off_t offset, void *buf, size_t count);
	KASSERT(vNode->vn_ops && vNode->vn_ops->read);
		dbg(DBG_PRINT, "(GRADING2A)\n");
	numOfReadBits = vNode->vn_ops->read(vNode, f->f_pos, buf, nbytes);
	// k3 // KASSERT(numOfReadBits >= 0);
		dbg(DBG_PRINT, "(GRADING2A)\n");

	// step 3: update f_pos
	if (numOfReadBits>0){
		f->f_pos += numOfReadBits;
		dbg(DBG_PRINT, "(GRADING3A)\n");
	}
	// step 4: fput() it (current file)
	fput(f);

	// step 5: return the number of bytes read, or an error 
	//if (numOfReadBits > 0) {
	dbg(DBG_PRINT, "(GRADING2A)\n");
	return numOfReadBits;
	//}
		  
}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * vn_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */
int
do_write(int fd, const void *buf, size_t nbytes)
{
//        NOT_YET_IMPLEMENTED("VFS: do_write");
//        return -1;

	struct file * f = NULL;
    struct vnode * vNode = NULL;
    int numOfWriteBits = 0; 

    // fd is not a valid file descriptor
    f= fget(fd); 
    if (f == NULL) {
		dbg(DBG_PRINT, "(GRADING2B)\n");
    	return -EBADF;
    }

    // fd is not open for write 
    // Q1: need to check FMODE_APPEND as well? 
	if (!((f->f_mode) & (FMODE_WRITE | FMODE_APPEND))) {
		fput(f);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	// get the vnode for this file object
	vNode = f->f_vnode;

	// Q2: should we check fd refers to a directory in this function? 
	// not reached
	//if (S_ISDIR(vNode->vn_mode)) {
	//	fput(f);
	//	dbg(DBG_PRINT, "(NEWPATH)\n");
	//	return -EISDIR;
	//}

	// append mode
	if (f->f_mode & FMODE_APPEND) {
		// should go to the position after the last char
		do_lseek(fd, 0, SEEK_END);
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	KASSERT(vNode->vn_ops && vNode->vn_ops->write);
		dbg(DBG_PRINT, "(GRADING2A)\n");

	if ((numOfWriteBits = vNode->vn_ops->write(vNode, f->f_pos, buf, nbytes)) >= 0){
		KASSERT((S_ISCHR(f->f_vnode->vn_mode)) ||
		        (S_ISBLK(f->f_vnode->vn_mode)) ||
		        ((S_ISREG(f->f_vnode->vn_mode)) && (f->f_pos <= f->f_vnode->vn_len))); // cursor must not go past end of file for these file types //
		dbg(DBG_PRINT, "(GRADING2A 3.a)\n");
		f->f_pos += numOfWriteBits;
		dbg(DBG_PRINT, "(GRADING2A)\n");
	}

	fput(f);
	dbg(DBG_PRINT, "(GRADING2A)\n");
	return numOfWriteBits;
/*
	file_t *f = fget(fd);
	if (f == NULL){
		return -EBADF;
	}
	KASSERT(f->f_vnode && f->f_vnode->vn_ops->write);
	if (!(f->f_mode & (FMODE_WRITE | FMODE_APPEND))){
		return -EBADF; // TODO: check
	}
   // write is successful and after cursor position has been updated //

	if ((ret = f->f_vnode->vn_ops->write (f->f_vnode, f->f_pos, buf, nbytes ) ) >= 0 ) {
                                 ...
                                 KASSERT((S_ISCHR(f->f_vnode->vn_mode)) ||
                                         (S_ISBLK(f->f_vnode->vn_mode)) ||
                                         ((S_ISREG(f->f_vnode->vn_mode)) && (f->f_pos <= f->f_vnode->vn_len))) // cursor must not go past end of file for these file types //
                             
	}
	
	return 0; // TODO: fix later 
*/
}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */
int
do_close(int fd)
{
//        NOT_YET_IMPLEMENTED("VFS: do_close");
//        return -1;
	struct file * f;
	if (fd < 0 || fd >= NFILES){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	f = curproc->p_files[fd];

	if (f == NULL) {
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	curproc->p_files[fd] = NULL;
	fput(f);
	dbg(DBG_PRINT, "(GRADING2A)\n");
	return 0;		  
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
//        NOT_YET_IMPLEMENTED("VFS: do_dup");
//        return -1;
	struct file * f = NULL;
	int nfd; 

	// check fd isn't an open file descriptor
	if (fd == -1){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	f = fget(fd);
	if (f == NULL) {
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	// how to determine the process has the maximum number of fd open

	nfd = get_empty_fd(curproc);
	// not reached 
	//if (nfd == -EMFILE) {
	//	fput(f);
	//	dbg(DBG_PRINT, "(NEWPATH)\n");
	//	return nfd;
	//}
	curproc->p_files[nfd] = curproc->p_files[fd];
	dbg(DBG_PRINT, "(GRADING2B)\n");
	return nfd;
}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
        // NOT_YET_IMPLEMENTED("VFS: do_dup2");
        // return -1;
	struct file *of = NULL; 
	//struct file *nf = NULL; // commented out due to compile warning

    // check ofd isn't an open file descriptor 
	if (ofd == -1 || ofd >= NFILES){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	// checking if it is valid range for new fd
	if (nfd == -1 || nfd >= NFILES){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
    of = fget(ofd); 
    if (of == NULL) { 
		  dbg(DBG_PRINT, "(GRADING2B)\n");
        return -EBADF; 
    }
	if (nfd == ofd){
		fput(of);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return nfd;
	}

    //nf = fget(nfd); // commented out due to compile warning

	 // at this point, nf can be null because this nfd is not used yet or nf can be pointing to a valid file obj
    //if (nf == NULL) { 
    //    return -EBADF; 
    //}
	 // not reached but keep for kernel 3
    //if (nf != NULL) { 
	 //  do_close(nfd); 
	 //  dbg(DBG_PRINT, "(NEWPATH)\n");
    //}

	curproc->p_files[nfd] = curproc->p_files[ofd];
	dbg(DBG_PRINT, "(GRADING2B)\n");
    return nfd; 
}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mknod(const char *path, int mode, unsigned devid)
{
        //NOT_YET_IMPLEMENTED("VFS: do_mknod");
        //need to check: vput(), ENOTDIR --> dir_namev will take care 
        size_t namelen;
        const char *name;
        vnode_t *dir_vnode;
        int ret;
        vnode_t *result;

        // not reached
		  //if(!S_ISCHR(mode) && !S_ISBLK(mode)){
		  // 	dbg(DBG_PRINT, "(NEWPATH)\n");
        //    return -EINVAL;
        //}

        ret = dir_namev(path, &namelen, &name, NULL, &dir_vnode);
		  // not reached
        //if(ret != 0){
		  // 	dbg(DBG_PRINT, "(NEWPATH)\n");
        //    return ret;
        //}

        ret = lookup(dir_vnode, name, namelen, &result);
		  // not reached
        //if(ret == 0){
		  //   vput(dir_vnode);
		  //   vput(result);
		  //   dbg(DBG_PRINT, "(NEWPATH)\n");
		  //   return -EEXIST;
        //}
		  //if (ret == -ENAMETOOLONG ){
        //        vput(dir_vnode); // need to check
		  // 		 dbg(DBG_PRINT, "(NEWPATH)\n");
        //        return ret;
		  //}
		  KASSERT(NULL != dir_vnode->vn_ops);
        KASSERT(NULL != dir_vnode->vn_ops->mknod); //(middle)/*dir_vnode is the directory vnode where you will create the target special file */
		  dbg(DBG_PRINT, "(GRADING2A 3.b)\n");
        ret = dir_vnode->vn_ops->mknod(dir_vnode, name, namelen, mode, (devid_t)devid);
  		  vput(dir_vnode); // parent dir
		  dbg(DBG_PRINT, "(GRADING2A)\n");
        return ret;
		  
		  
}

/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mkdir(const char *path)
{
//        NOT_YET_IMPLEMENTED("VFS: do_mkdir");
//        return -1;
//need to check: ENOENT,ENOTDIR, ENAMETOOLONG --> dir_namev will take care
        size_t namelen;
        const char *name;
        vnode_t *dir_vnode;
        int ret;
        vnode_t *result;

        ret = dir_namev(path, &namelen, &name, NULL, &dir_vnode);

        if(ret != 0){
				dbg(DBG_PRINT, "(GRADING2B)\n");
         	return ret;
        }
		  if (!S_ISDIR(dir_vnode->vn_mode)){
			  vput(dir_vnode);
			  dbg(DBG_PRINT, "(GRADING2B)\n");
			  return -ENOTDIR;
		  }

        ret = lookup(dir_vnode, name, namelen, &result);

        if(ret == 0){
                vput(dir_vnode); // need to check
                vput(result); // need to check
			  		 dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EEXIST;
        }
		  if (ret == -ENAMETOOLONG ){
                vput(dir_vnode); // need to check
			  		 dbg(DBG_PRINT, "(GRADING2B)\n");
                return ret;
		  }
		  KASSERT(NULL != dir_vnode->vn_ops);
        KASSERT(NULL != dir_vnode->vn_ops->mkdir); //(middle)/*dir_vnode is the directory vnode where you will create the target directory */
		  dbg(DBG_PRINT, "(GRADING2A 3.c)\n");
        ret = dir_vnode->vn_ops->mkdir(dir_vnode, name, namelen);
        vput(dir_vnode); // need to check
		  dbg(DBG_PRINT, "(GRADING2A)\n");
        return ret;


}

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its final component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_rmdir(const char *path)
{
//        NOT_YET_IMPLEMENTED("VFS: do_rmdir");
//        return -1;
        //need to check: ENOTDIR, ENOENT, ENAMETOOLONG --> dir_namev will take care
        size_t namelen;
        const char *name;
        vnode_t *dir_vnode, *result;
        int ret;

        ret = dir_namev(path, &namelen, &name, NULL, &dir_vnode);

        if(ret != 0){
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
                return ret;
        }
		  if (!strncmp(name, ".", namelen) ){
			  vput(dir_vnode);
		  	  dbg(DBG_PRINT, "(GRADING2B)\n");
			  return -EINVAL;
		  }
		  if (!strncmp(name, "..", namelen) ){
			  vput(dir_vnode);
		     dbg(DBG_PRINT, "(GRADING2B)\n");
			  return -ENOTEMPTY;
		  }
		  ret = lookup(dir_vnode, name, namelen, &result);

        if(ret != 0){
				vput(dir_vnode);
		  	   dbg(DBG_PRINT, "(GRADING2B)\n");
				return ret;
		  }
		  // not reached
		  //if (!S_ISDIR(dir_vnode->vn_mode)){
		  //   vput(dir_vnode);
		  //	  dbg(DBG_PRINT, "(NEWPATH)\n");
		  //   return -ENOTDIR;
		  //}

		  KASSERT(NULL != dir_vnode->vn_ops);
        KASSERT(NULL != dir_vnode->vn_ops->rmdir); //(middle)/*please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                                                             /*dir_vnode is the directory vnode where you will remove the target directory */
		  dbg(DBG_PRINT, "(GRADING2A 3.d)\n");
		  dbg(DBG_PRINT, "(GRADING2B)\n");
			
        ret = dir_vnode->vn_ops->rmdir(dir_vnode, name, namelen);
        vput(dir_vnode); //need to check
		  vput(result);
		  dbg(DBG_PRINT, "(GRADING2B)\n");
        return ret;
}

/*
 * Similar to do_rmdir, but for files.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EPERM
 *        path refers to a directory.
 *      o ENOENT
 *        Any component in path does not exist, including the element at the
 *        very end.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
//        NOT_YET_IMPLEMENTED("VFS: do_unlink");
       //need to check: ENOTDIR, EPERM, ENOENT, ENAMETOOLONG
        size_t namelen;
        const char *name;
        vnode_t *dir_vnode;
        int ret;
        vnode_t *result;

        ret = dir_namev(path, &namelen, &name, NULL, &dir_vnode);
        // not reached
		  //if(ret != 0){
		  //			 dbg(DBG_PRINT, "(NEWPATH)\n");
        //        return ret;
        //}
        ret = lookup(dir_vnode, name, namelen, &result);
        if(ret != 0){
                vput(dir_vnode); // need to check
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
                return ret;
        }
		  if (S_ISDIR(result->vn_mode)){
                vput(dir_vnode); // need to check
                vput(result); // need to check
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EPERM;
		  }
		  KASSERT(NULL != dir_vnode->vn_ops);
        KASSERT(NULL != dir_vnode->vn_ops->unlink);//(middle)/*please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                                                /*dir_vnode is the directory vnode where you will unlink the target file */
		  dbg(DBG_PRINT, "(GRADING2A 3.e)\n");
		  dbg(DBG_PRINT, "(GRADING2B)\n");
        ret = dir_vnode->vn_ops->unlink(dir_vnode, name, namelen);
        vput(dir_vnode); // need to check
        vput(result); // need to check
		  dbg(DBG_PRINT, "(GRADING2B)\n");
        return ret;
}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EPERM
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
//        NOT_YET_IMPLEMENTED("VFS: do_link");
//        return -1;
        //*Remember to vput the vnodes returned from open_namev and dir_namev.
        //need to check: ENOENT, ENOTDIR, EPERM, ENAMETOOLONG
        vnode_t *fromNode;
        vnode_t *dir_vnode;
        vnode_t *result;
        int ret;
        size_t namelen;
        const char *name;

        ret = open_namev(from, 0, &fromNode, NULL);

        if(ret != 0){
                //vput(fromNode) need to check
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
                return ret;
        }
		  // not reached
		  if (S_ISDIR(fromNode->vn_mode)){
		     vput(fromNode);
		  	  dbg(DBG_PRINT, "(GRADING3D 1)\n");
		     return -EPERM;
		  }
        ret = dir_namev(to, &namelen, &name, NULL, &dir_vnode);
		  // not reached
        //if(ret != 0){
        //        //vput(dir_vnode) need to check
        //        vput(fromNode); // need to check
		  //			 dbg(DBG_PRINT, "(NEWPATH)\n");
        //        return ret;
        //}

        ret = lookup(dir_vnode, name, namelen, &result);

		  // not reached
        if(ret == 0){
                vput(dir_vnode); // need to check
                vput(fromNode); // need to check
		   		 vput(result);
		  			 dbg(DBG_PRINT, "(GRADING3D 1)\n");
                return -EEXIST;
        }
		  if (ret == -ENAMETOOLONG ){
			  		 vput(fromNode);
                vput(dir_vnode); // need to check
		  			 dbg(DBG_PRINT, "(GRADING3D 1)\n");
                return ret;
		  }
		  dbg(DBG_PRINT, "(GRADING2B)\n");
		  if(!S_ISDIR(dir_vnode->vn_mode)){
                vput(dir_vnode); // need to check
                vput(fromNode); // need to check
					 //vput(result); // doesn't exist yet
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
		  }
// for kernel 3 
//#ifdef __VM__
		  KASSERT(NULL != dir_vnode->vn_ops);
		  KASSERT(NULL != dir_vnode->vn_ops->link);
		  dbg(DBG_PRINT, "(GRADING2D)\n");
        ret = dir_vnode->vn_ops->link(fromNode, dir_vnode, name, namelen);
        vput(dir_vnode); // need to check
        //vput(result);    // shouldn't exist
        vput(fromNode);   // need to check
		  dbg(DBG_PRINT, "(GRADING2D)\n");
//#endif		  
        return ret;		  
}

/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
        //NOT_YET_IMPLEMENTED("VFS: do_rename");
        //return -1;
	    KASSERT(NULL != oldname);
	    KASSERT(NULL != newname);
		 dbg(DBG_PRINT, "(GRADING2B)\n"); // self
		 int ret = 0;
		 dbg(DBG_PRINT, "(GRADING2B)\n"); // self
	    if ((ret = do_link(oldname, newname)) != 0){
		  	 dbg(DBG_PRINT, "(GRADING2B)\n");
			 return ret;
		 }
		 // need to take care for kernel 3
		 dbg(DBG_PRINT, "(GRADING2D)\n"); // not reached 
	    return do_unlink(oldname);	
}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
        //NOT_YET_IMPLEMENTED("VFS: do_chdir");
        //return -1;
	KASSERT(NULL != path);
	dbg(DBG_PRINT, "(GRADING2B)\n"); // self
	vnode_t *oldcwd = curproc -> p_cwd;
    vnode_t *base = NULL;
    //vnode_t **res_pvnode = NULL;
    vnode_t *res_vnode = NULL;
    //const char** name = &path;
    //size_t *namelen = strlen(path);
    //dir_namev(path, namelen, name, base, res_pvnode);
    int res = open_namev(path, 0, &res_vnode, base);
    //int res = lookup (*res_pvnode, *name, strlen(*name), res_vnode);
    if (res == 0){
			if (!S_ISDIR(res_vnode->vn_mode)){
		   	vput(res_vnode);
		  		dbg(DBG_PRINT, "(GRADING2B)\n");
				return -ENOTDIR;
			}

    	curproc -> p_cwd = res_vnode;
	    vput(oldcwd);
		 dbg(DBG_PRINT, "(GRADING2B)\n");
    }else{
		dbg(DBG_PRINT, "(GRADING2B)\n");
    	return res;
    }
	 dbg(DBG_PRINT, "(GRADING2B)\n");
    return 0;
}

/* Call the readdir vn_op on the given fd, filling in the given dirent_t*.
 * If the readdir vn_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */
int
do_getdent(int fd, struct dirent *dirp)
{
        //NOT_YET_IMPLEMENTED("VFS: do_getdent");
        //return -1;
	KASSERT(NULL != dirp);
	dbg(DBG_PRINT, "(GRADING2B)\n"); // self
	if (fd == -1){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	
	file_t *f = fget(fd);
	if (f == NULL){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
    if (!S_ISDIR(f->f_vnode->vn_mode)) {
        fput(f);
		  dbg(DBG_PRINT, "(GRADING2B)\n");
        return -ENOTDIR;
    }
	
    KASSERT(NULL != f->f_vnode);
    KASSERT(NULL != f->f_vnode->vn_ops);
	 KASSERT(f->f_vnode->vn_ops->readdir);
	 dbg(DBG_PRINT, "(GRADING2B)\n"); // self 

	int res = f->f_vnode->vn_ops->readdir(f->f_vnode, f->f_pos, dirp); 
	KASSERT(res >= 0);
	dbg(DBG_PRINT, "(GRADING2B)\n");
	f->f_pos += res;
//	if (res < 0){
//        fput(f);
//		return res;
//	}
   fput(f);
	if (res > 0){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return sizeof(dirent_t);
	}
	dbg(DBG_PRINT, "(GRADING2B)\n");
	return res;

}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 */
int
do_lseek(int fd, int offset, int whence)
{
        //NOT_YET_IMPLEMENTED("VFS: do_lseek");
        //return -1;
	int newpos = 0;
	if (fd == -1){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	if ((whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) ){
//		(f->f_pos + offset < 0) || 	(f->f_pos + offset >= f->f_vnode->vn_len) ){
		 dbg(DBG_PRINT, "(GRADING2B)\n");
	    return -EINVAL;
	}
	
	file_t *f = fget(fd);
	if (f == NULL){
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	if (whence == SEEK_SET) {
		newpos = offset;
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	else if (whence == SEEK_CUR){
		newpos = f->f_pos + offset;
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	else{
		newpos = f->f_vnode->vn_len + offset;
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
		
	//if (newpos < 0 || newpos > f->f_vnode->vn_len){
	if (newpos < 0){
		fput(f);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}
	f->f_pos = newpos;
	fput(f);
	dbg(DBG_PRINT, "(GRADING2B)\n");

	return newpos;

}

/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o EINVAL
 *        path is an empty string.
 */
int
do_stat(const char *path, struct stat *buf)
{
        //NOT_YET_IMPLEMENTED("VFS: do_stat");
        //return -1;
	KASSERT(NULL != path);
	KASSERT(NULL != buf);
	dbg(DBG_PRINT, "(GRADING2B)\n");
    vnode_t *base = NULL;
    //vnode_t **res_pvnode = NULL;
    vnode_t *res_vnode = NULL;
    //const char** name = &path;
    //size_t *namelen = strlen(path);
    int res = open_namev(path, 0, &res_vnode, base);
    //dir_namev(path, namelen, name, base, res_pvnode);
    //int res = lookup (*res_pvnode, *name, strlen(*name), res_vnode);
    if (res == 0){
		 KASSERT(res_vnode && res_vnode->vn_ops);
		 
		 KASSERT(NULL != res_vnode->vn_ops->stat);
		  			 dbg(DBG_PRINT, "(GRADING2A 3.f)\n");
		  			 dbg(DBG_PRINT, "(GRADING2B)\n");
    	res = res_vnode->vn_ops->stat(res_vnode,buf);// need to clearify
    	vput(res_vnode);
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return res;
    }else{
		dbg(DBG_PRINT, "(GRADING2B)\n");
    	return res;
    }
}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
