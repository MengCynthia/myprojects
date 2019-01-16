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
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function.
 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{
	int ret=0;
//        NOT_YET_IMPLEMENTED("VFS: lookup");
	KASSERT(NULL != dir); /* the "dir" argument must be non-NULL */
	KASSERT(NULL != name); /* the "name" argument must be non-NULL */
	KASSERT(NULL != result); /* the "result" argument must be non-NULL */
	dbg(DBG_PRINT, "(GRADING2A 2.a)\n");
	
	KASSERT(dir->vn_fs && dir->vn_fs->fs_op);
	dbg(DBG_PRINT, "(GRADING2A)\n");

	
	if (len == 0){
		//if (dir == vfs_root_vn){
			KASSERT(dir == vfs_root_vn);
			*result = dir;
			vref(dir);
			dbg(DBG_PRINT, "(GRADING2B)\n");
			return 0;
		//}
		// not reached
		//*result = NULL;
		//dbg(DBG_PRINT, "NEWPATH\n");
		//return -EINVAL;
	}
	if (len > NAME_LEN - 1){
		*result = NULL;
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENAMETOOLONG;
	}
	if ( dir->vn_ops->lookup == NULL ) { 
		*result = NULL;
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTDIR;
	}
	ret = dir->vn_ops->lookup(dir, name, len, result) ;
    dbg(DBG_PRINT, "(GRADING2A)\n");

	// TODO: incorrect because dir will be vnode of dir, so name won't contain the path name anymore...
	// TODO: unimplemented for now
	//if (strncmp("/dev/", name, 5 ) == 0 && result == NULL){
	//	return -ENXIO;
	//}
	return ret;
}

int isSlashOnly(const char *str){
	int res=0;
	const char *ptr= str;
	KASSERT( *str != '\0' );
	dbg(DBG_PRINT, "(GRADING2A)\n");
	while ( *ptr != '\0' ){
		if (*ptr != '/'){
			dbg(DBG_PRINT, "(GRADING2A)\n");
			return res;
		}
		ptr++;
		dbg(DBG_PRINT, "(GRADING2A)\n");
	}
	dbg(DBG_PRINT, "(GRADING2B)\n");
	return 1;
}
/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
 */
int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{
//        NOT_YET_IMPLEMENTED("VFS: dir_namev");
	KASSERT(NULL != pathname); /* the "pathname" argument must be non-NULL */
	KASSERT(NULL != namelen); /* the "namelen" argument must be non-NULL */
	KASSERT(NULL != name); /* the "name" argument must be non-NULL */
	KASSERT(NULL != res_vnode); /* the "res_vnode" argument must be non-NULL */
	dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
	
	const char *ptr1 = pathname, *ptr2 = pathname;
	vnode_t *temp_vnode = NULL;
	int ret = 0; // , seen = 0;

	vnode_t *dir_vnode = NULL;
	if (pathname[0] == '/'){
		dir_vnode = vfs_root_vn; 
	dbg(DBG_PRINT, "(GRADING2A)\n");
	}
	else if (base == NULL) {
		dir_vnode = curproc->p_cwd;
	dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	// not reached
	//else{
	//	dir_vnode = base;
	//dbg(DBG_PRINT, "(NEWPATH)\n");
	//}
	// TODO: KASSERT for other cases? base != NULL or pathname[0] != '/'
	KASSERT(NULL != dir_vnode); /* pathname resolution must start with a valid directory */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
/*
check if the path exist. if so, get parent vnode, handle name parsing. if not, return error
parse first into parent and current directory, setup name, namelen, lookup parent
*/
	
	if (!*ptr1){
		*name = NULL;
		*namelen = 0;
		*res_vnode = NULL;
		dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}
	// when lookup is called, ptr1 should not start with '/'
	vref(dir_vnode);
	dbg(DBG_PRINT, "(GRADING2A)\n");
	while (*ptr1){
		// special case '/' only
		if (*ptr2 == '\0'){ // end of the string
			dbg(DBG_PRINT, "(GRADING2A)\n");
			if (*ptr1 != '/'){ // take care the case of no '/' at all or every level of comp name is valid
				*name = ptr1;
				*namelen = ptr2 - ptr1;
				*res_vnode = dir_vnode;
				dbg(DBG_PRINT, "(GRADING2A)\n");
				//vref(dir_vnode);
				return ret;
			}
			else { // if *ptr1 == '/'
				// starting with "/" or "////" case
				//dbg(DBG_PRINT, "(GRADING2B)\n");
				//if (dir_vnode == vfs_root_vn){ // may need to change it for kernel 3
				KASSERT(dir_vnode == vfs_root_vn);
				dbg(DBG_PRINT, "(GRADING2B)\n");
					*name = ptr2;
					*namelen = 0;
					*res_vnode = dir_vnode;
					//vref(dir_vnode); // check
					dbg(DBG_PRINT, "(GRADING2B)\n");
					return 0;
				//}
				//dbg(DBG_PRINT, "NEWPATH\n");
				// else means trailing ///, in this case, we have already found in the previous lookup call
				//vref(*res_vnode); // check
				//return 0; // also that means previous look up is ok
			}
			//dbg(DBG_PRINT, "NEWPATH\n");
			//break; // reach end of pathname
		}
		else if (*ptr2 == '/'){
			if (*ptr1 == '/'){ // take care of the consequtive '/'
				ptr1 = ptr2;
				ptr2++; 
				dbg(DBG_PRINT, "(GRADING2A)\n");
			}
			else { // when *ptr1 != '/' means there must be some other chars between *ptr1 & *ptr2
				// if only trailing '/', return result
				// else 
				if (isSlashOnly(ptr2)){ 
					*name = ptr1;
					*namelen = ptr2 - ptr1;
					*res_vnode = dir_vnode;
					dbg(DBG_PRINT, "(GRADING2B)\n");
					//vref(dir_vnode);
					return 0;
				}
				else{
					if ( (ret = lookup(dir_vnode, ptr1, ptr2 - ptr1, &temp_vnode)) == 0) {
						// in case it is the final level, save these first
						// there is next level, so just need to  update before return
						//*namelen = ptr2 - ptr1;
						//*name = ptr1;
						//*res_vnode = dir_vnode; 
						// Question: need to consider parent like the above? *res_vnode = dir_vnode; // done
						
						// start on next level
						ptr1 = ptr2++;
						// QUESTION: do we need to vput dir_vnode? we can't tell
						vput(dir_vnode); // TODO: check? what if we need to return this one?
						dir_vnode = temp_vnode;
						dbg(DBG_PRINT, "(GRADING2A)\n");
					}
					else { 
						// QUESTION: do we need to vput dir_vnode? Need to set res_vnode? what about other return values?
						*name = NULL;
						*namelen = 0;
						*res_vnode = NULL;
						vput(dir_vnode);
						dbg(DBG_PRINT, "(GRADING2B)\n");
						return ret; // error case - invalid path
					}
					dbg(DBG_PRINT, "(GRADING2A)\n");
				}
				dbg(DBG_PRINT, "(GRADING2A)\n");
			}
			dbg(DBG_PRINT, "(GRADING2A)\n");
			//seen = 1;
		}
		else { // when ptr2 points to non '/' or non '\0'
			if (*ptr1 == '/'){
				ptr1 = ptr2;
				dbg(DBG_PRINT, "(GRADING2A)\n");
			}
			++ptr2;
			dbg(DBG_PRINT, "(GRADING2A)\n");
			//seen = 0;
		}
		dbg(DBG_PRINT, "(GRADING2A)\n");
	}
	// not reachable but to fix comile warning
	return ret;
	//if (!seen){
	//	*res_vnode = dir_vnode; // parent
	//	*namelen = ptr2 - ptr1;
	//	*name = ptr1; 
	//	return ret;
	//}
	// not reached
	//vput(dir_vnode);
	//dbg(DBG_PRINT, "(NEWPATH)\n");
	//return ret; // Question: check
}

/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fcntl.h>.  If the O_CREAT flag is specified and the file does
 * not exist, call create() in the parent directory vnode. However, if the
 * parent directory itself does not exist, this function should fail - in all
 * cases, no files or directories other than the one at the very end of the path
 * should be created.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)
{
//        NOT_YET_IMPLEMENTED("VFS: open_namev");
	size_t namelen;
	const char *name = NULL;
	int ret;
	vnode_t *dir_vnode;

	if ((ret = dir_namev (pathname, &namelen, &name, base, &dir_vnode)) == 0){ // Found parent dir!
		//vput(dir_vnode);
		dbg(DBG_PRINT, "(GRADING2A)\n");
		if ((ret = lookup(dir_vnode, name, namelen, res_vnode)) != 0){ // New file
/* please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
/* dir_vnode is a directory vnode of the file you are trying to open */
			// may not work if we want to use this function to look up dir	
			//if (*(name+namelen+1) == '/' ){ // incorrect path name
			//	vput(dir_vnode);
			//	*res_vnode = NULL;
			//	return -ENOTDIR;
			//}
			if (ret == -ENAMETOOLONG ){
				vput(dir_vnode);
				*res_vnode = NULL;
				dbg(DBG_PRINT, "(GRADING2B)\n");
            return ret;
		   }
			if (!S_ISDIR(dir_vnode->vn_mode)){
		   	vput(dir_vnode);
				*res_vnode = NULL;
			    dbg(DBG_PRINT, "(GRADING2B)\n");
				return -ENOTDIR;
			}
			dbg(DBG_PRINT, "(GRADING2B)\n");
			// may need it for kernel 3
			//if (flag & (O_CREAT | O_TRUNC) ) { /* if file does not exist inside dir_vnode, need to make sure you can create the file */
			if (flag & O_CREAT ) { /* if file does not exist inside dir_vnode, need to make sure you can create the file */
				//if (!S_ISDIR(dir_vnode->vn_mode)){
			   //	vput(dir_vnode);
				//	*res_vnode = NULL;
				//    dbg(DBG_PRINT, "(GRADING2B)\n");
				//	return -ENOTDIR;
				//}
				// it must be a directory before calling create
				KASSERT(NULL != dir_vnode->vn_ops->create);
				dbg(DBG_PRINT, "(GRADING2A 2.c)\n");
				dbg(DBG_PRINT, "(GRADING2B)\n");
				// create in ramfs only creates a file
				ret = dir_vnode->vn_ops->create(dir_vnode, name, namelen, res_vnode);
				vput(dir_vnode);
				dbg(DBG_PRINT, "(GRADING2B)\n");
				return ret;
			}	
			else {
			// TODO: what happen to other flag? - error
			// TODO: check clean up task
			   vput(dir_vnode);
				*res_vnode = NULL;
				dbg(DBG_PRINT, "(GRADING2B)\n");
				return ret;
			}
		}
		else { // else case: found existing file
			vput(dir_vnode);
			if (*(name+namelen) == '/' && !S_ISDIR((*res_vnode)->vn_mode) ){ // incorrect path name
				vput(*res_vnode);
				*res_vnode = NULL;
				dbg(DBG_PRINT, "(GRADING2B)\n");
				return -ENOTDIR;
			}
			// not reached
			//if ((flag & O_CREAT)) {
			////	vput(dir_vnode);
			//	vput(*res_vnode);
			//	*res_vnode = NULL;
			//	dbg(DBG_PRINT, "(NEWPATH)\n");
			//	return -EEXIST;
			//}
			dbg(DBG_PRINT, "(GRADING2A)\n");
			return ret; // found file and not O_CREAT
		}
	}
	//else { // doesn't find the parent directory
		// TODO: error handling?
    dbg(DBG_PRINT, "(GRADING2B)\n");
		return ret;
	//}
}

#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */
