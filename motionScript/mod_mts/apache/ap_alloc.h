/* Copyright 1999-2004 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APACHE_ALLOC_H
#define APACHE_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Resource allocation routines...
 *
 * designed so that we don't have to keep track of EVERYTHING so that
 * it can be explicitly freed later (a fundamentally unsound strategy ---
 * particularly in the presence of die()).
 *
 * Instead, we maintain pools, and allocate items (both memory and I/O
 * handlers) from the pools --- currently there are two, one for per
 * transaction info, and one for config info.  When a transaction is over,
 * we can delete everything in the per-transaction pool without fear, and
 * without thinking too hard about it either.
 *
 * rst
 */

/* Arenas for configuration info and transaction info
 * --- actual layout of the pool structure is private to 
 * alloc.c.  
 */

 /* Need declaration of DIR on Win32 */
#ifdef WIN32
#include "readdir.h"
#endif

typedef struct pool pool;
typedef struct pool ap_pool;

API_EXPORT(pool *) ap_init_alloc(void);		/* Set up everything */
void ap_cleanup_alloc(void);
API_EXPORT(pool *) ap_make_sub_pool(pool *);	/* All pools are subpools of permanent_pool */
API_EXPORT(void) ap_destroy_pool(pool *);

/* pools have nested lifetimes -- sub_pools are destroyed when the
 * parent pool is cleared.  We allow certain liberties with operations
 * on things such as tables (and on other structures in a more general
 * sense) where we allow the caller to insert values into a table which
 * were not allocated from the table's pool.  The table's data will
 * remain valid as long as all the pools from which its values are
 * allocated remain valid.
 *
 * For example, if B is a sub pool of A, and you build a table T in
 * pool B, then it's safe to insert data allocated in A or B into T
 * (because B lives at most as long as A does, and T is destroyed when
 * B is cleared/destroyed).  On the other hand, if S is a table in
 * pool A, it is safe to insert data allocated in A into S, but it
 * is *not safe* to insert data allocated from B into S... because
 * B can be cleared/destroyed before A is (which would leave dangling
 * pointers in T's data structures).
 *
 * In general we say that it is safe to insert data into a table T
 * if the data is allocated in any ancestor of T's pool.  This is the
 * basis on which the POOL_DEBUG code works -- it tests these ancestor
 * relationships for all data inserted into tables.  POOL_DEBUG also
 * provides tools (ap_find_pool, and ap_pool_is_ancestor) for other
 * folks to implement similar restrictions for their own data
 * structures.
 *
 * However, sometimes this ancestor requirement is inconvenient --
 * sometimes we're forced to create a sub pool (such as through
 * ap_sub_req_lookup_uri), and the sub pool is guaranteed to have
 * the same lifetime as the parent pool.  This is a guarantee implemented
 * by the *caller*, not by the pool code.  That is, the caller guarantees
 * they won't destroy the sub pool individually prior to destroying the
 * parent pool.
 *
 * In this case the caller must call ap_pool_join() to indicate this
 * guarantee to the POOL_DEBUG code.  There are a few examples spread
 * through the standard modules.
 */
#ifndef POOL_DEBUG
#ifdef ap_pool_join
#undef ap_pool_join
#endif
#define ap_pool_join(a,b)
#else
API_EXPORT(void) ap_pool_join(pool *p, pool *sub);
API_EXPORT(pool *) ap_find_pool(const void *ts);
API_EXPORT(int) ap_pool_is_ancestor(pool *a, pool *b);
#endif

/* Clearing out EVERYTHING in an pool... destroys any sub-pools */

API_EXPORT(void) ap_clear_pool(struct pool *);

/* Preparing for exec() --- close files, etc., but *don't* flush I/O
 * buffers, *don't* wait for subprocesses, and *don't* free any memory.
 */

API_EXPORT(void) ap_cleanup_for_exec(void);

/* routines to allocate memory from an pool... */

API_EXPORT(void *) ap_palloc(struct pool *, int nbytes);
API_EXPORT(void *) ap_pcalloc(struct pool *, int nbytes);
API_EXPORT(char *) ap_pstrdup(struct pool *, const char *s);
/* make a nul terminated copy of the n characters starting with s */
API_EXPORT(char *) ap_pstrndup(struct pool *, const char *s, int n);
API_EXPORT_NONSTD(char *) ap_pstrcat(struct pool *,...);	/* all '...' must be char* */
API_EXPORT_NONSTD(char *) ap_psprintf(struct pool *, const char *fmt, ...)
    __attribute__((format(printf,2,3)));
API_EXPORT(char *) ap_pvsprintf(struct pool *, const char *fmt, va_list);

/* array and alist management... keeping lists of things.
 * Common enough to want common support code ...
 */

typedef struct {
    ap_pool *pool;
    int elt_size;
    int nelts;
    int nalloc;
    char *elts;
} array_header;

API_EXPORT(array_header *) ap_make_array(pool *p, int nelts, int elt_size);
API_EXPORT(void *) ap_push_array(array_header *);
API_EXPORT(void) ap_array_cat(array_header *dst, const array_header *src);
API_EXPORT(array_header *) ap_append_arrays(pool *, const array_header *,
					 const array_header *);

/* ap_array_pstrcat generates a new string from the pool containing
 * the concatenated sequence of substrings referenced as elements within
 * the array.  The string will be empty if all substrings are empty or null,
 * or if there are no elements in the array.
 * If sep is non-NUL, it will be inserted between elements as a separator.
 */
API_EXPORT(char *) ap_array_pstrcat(pool *p, const array_header *arr,
                                    const char sep);

/* copy_array copies the *entire* array.  copy_array_hdr just copies
 * the header, and arranges for the elements to be copied if (and only
 * if) the code subsequently does a push or arraycat.
 */

API_EXPORT(array_header *) ap_copy_array(pool *p, const array_header *src);
API_EXPORT(array_header *) ap_copy_array_hdr(pool *p, const array_header *src);


/* Tables.  Implemented alist style, for now, though we try to keep
 * it so that imposing a hash table structure on top in the future
 * wouldn't be *too* hard...
 *
 * Note that key comparisons for these are case-insensitive, largely
 * because that's what's appropriate and convenient everywhere they're
 * currently being used...
 */

typedef struct table table;

typedef struct {
    char *key;		/* maybe NULL in future;
			 * check when iterating thru table_elts
			 */
    char *val;
} table_entry;

API_EXPORT(table *) ap_make_table(pool *p, int nelts);
API_EXPORT(table *) ap_copy_table(pool *p, const table *);
API_EXPORT(void) ap_clear_table(table *);
API_EXPORT(const char *) ap_table_get(const table *, const char *);
API_EXPORT(void) ap_table_set(table *, const char *name, const char *val);
API_EXPORT(void) ap_table_setn(table *, const char *name, const char *val);
API_EXPORT(void) ap_table_merge(table *, const char *name, const char *more_val);
API_EXPORT(void) ap_table_mergen(table *, const char *name, const char *more_val);
API_EXPORT(void) ap_table_unset(table *, const char *key);
API_EXPORT(void) ap_table_add(table *, const char *name, const char *val);
API_EXPORT(void) ap_table_addn(table *, const char *name, const char *val);
API_EXPORT_NONSTD(void) ap_table_do(int (*comp) (void *, const char *, const char *), 
                                    void *rec, const table *t,...);

API_EXPORT(table *) ap_overlay_tables(pool *p, const table *overlay, const table *base);

/* Conceptually, ap_overlap_tables does this:

    array_header *barr = ap_table_elts(b);
    table_entry *belt = (table_entry *)barr->elts;
    int i;

    for (i = 0; i < barr->nelts; ++i) {
	if (flags & AP_OVERLAP_TABLES_MERGE) {
	    ap_table_mergen(a, belt[i].key, belt[i].val);
	}
	else {
	    ap_table_setn(a, belt[i].key, belt[i].val);
	}
    }

    Except that it is more efficient (less space and cpu-time) especially
    when b has many elements.

    Notice the assumptions on the keys and values in b -- they must be
    in an ancestor of a's pool.  In practice b and a are usually from
    the same pool.
*/
#define AP_OVERLAP_TABLES_SET	(0)
#define AP_OVERLAP_TABLES_MERGE	(1)
API_EXPORT(void) ap_overlap_tables(table *a, const table *b, unsigned flags);

/* XXX: these know about the definition of struct table in alloc.c.  That
 * definition is not here because it is supposed to be private, and by not
 * placing it here we are able to get compile-time diagnostics from modules
 * written which assume that a table is the same as an array_header. -djg
 */
#define ap_table_elts(t) ((array_header *)(t))
#define ap_is_empty_table(t) (((t) == NULL)||(((array_header *)(t))->nelts == 0))

/* routines to remember allocation of other sorts of things...
 * generic interface first.  Note that we want to have two separate
 * cleanup functions in the general case, one for exec() preparation,
 * to keep CGI scripts and the like from inheriting access to things
 * they shouldn't be able to touch, and one for actually cleaning up,
 * when the actual server process wants to get rid of the thing,
 * whatever it is.  
 *
 * kill_cleanup disarms a cleanup, presumably because the resource in
 * question has been closed, freed, or whatever, and it's scarce
 * enough to want to reclaim (e.g., descriptors).  It arranges for the
 * resource not to be cleaned up a second time (it might have been
 * reallocated).  run_cleanup does the same, but runs it first.
 *
 * Cleanups are identified for purposes of finding & running them off by the
 * plain_cleanup and data, which should presumably be unique.
 *
 * NB any code which invokes register_cleanup or kill_cleanup directly
 * is a critical section which should be guarded by block_alarms() and
 * unblock_alarms() below...
 *
 * ap_register_cleanup_ex provided to allow for an optional "cleanup"
 * to be run at call-time for things like setting CLOSEXEC flags
 * on fd's or whatever else may make sense.
 */

API_EXPORT(void) ap_register_cleanup(pool *p, void *data,
				     void (*plain_cleanup) (void *),
				     void (*child_cleanup) (void *));
API_EXPORT(void) ap_register_cleanup_ex(pool *p, void *data,
				      void (*plain_cleanup) (void *),
				      void (*child_cleanup) (void *),
				      int (*magic_cleanup) (void *));

API_EXPORT(void) ap_kill_cleanup(pool *p, void *data, void (*plain_cleanup) (void *));
API_EXPORT(void) ap_run_cleanup(pool *p, void *data, void (*cleanup) (void *));

/* A "do-nothing" cleanup, for register_cleanup; it's faster to do
 * things this way than to test for NULL. */
API_EXPORT_NONSTD(void) ap_null_cleanup(void *data);

/* The time between when a resource is actually allocated, and when it
 * its cleanup is registered is a critical section, during which the
 * resource could leak if we got interrupted or timed out.  So, anything
 * which registers cleanups should bracket resource allocation and the
 * cleanup registry with these.  (This is done internally by run_cleanup).
 *
 * NB they are actually implemented in http_main.c, since they are bound
 * up with timeout handling in general...
 */

#ifdef TPF
#define ap_block_alarms() (0)
#define ap_unblock_alarms() (0)
#else
API_EXPORT(void) ap_block_alarms(void);
API_EXPORT(void) ap_unblock_alarms(void);
#endif /* TPF */

/* Common cases which want utility support..
 * the note_cleanups_for_foo routines are for 
 */

API_EXPORT(FILE *) ap_pfopen(struct pool *, const char *name, const char *fmode);
API_EXPORT(FILE *) ap_pfdopen(struct pool *, int fd, const char *fmode);
API_EXPORT(int) ap_popenf(struct pool *, const char *name, int flg, int mode);
API_EXPORT(int) ap_popenf_ex(struct pool *, const char *name, int flg,
                             int mode, int domagic);

API_EXPORT(void) ap_note_cleanups_for_file(pool *, FILE *);
API_EXPORT(void) ap_note_cleanups_for_file_ex(pool *, FILE *, int);
API_EXPORT(void) ap_note_cleanups_for_fd(pool *, int);
API_EXPORT(void) ap_note_cleanups_for_fd_ex(pool *, int, int);
#ifdef WIN32
API_EXPORT(void) ap_note_cleanups_for_h(pool *, HANDLE);
#endif
API_EXPORT(void) ap_kill_cleanups_for_fd(pool *p, int fd);

API_EXPORT(void) ap_note_cleanups_for_socket(pool *, int);
API_EXPORT(void) ap_note_cleanups_for_socket_ex(pool *, int, int);
API_EXPORT(void) ap_kill_cleanups_for_socket(pool *p, int sock);
API_EXPORT(int) ap_psocket(pool *p, int, int, int);
API_EXPORT(int) ap_psocket_ex(pool *p, int, int, int, int);
API_EXPORT(int) ap_pclosesocket(pool *a, int sock);

API_EXPORT(regex_t *) ap_pregcomp(pool *p, const char *pattern, int cflags);
API_EXPORT(void) ap_pregfree(pool *p, regex_t * reg);

/* routines to note closes... file descriptors are constrained enough
 * on some systems that we want to support this.
 */

API_EXPORT(int) ap_pfclose(struct pool *, FILE *);
API_EXPORT(int) ap_pclosef(struct pool *, int fd);
#ifdef WIN32
API_EXPORT(int) ap_pcloseh(struct pool *, HANDLE hDevice);
#endif

/* routines to deal with directories */
API_EXPORT(DIR *) ap_popendir(pool *p, const char *name);
API_EXPORT(void) ap_pclosedir(pool *p, DIR * d);

/* ... even child processes (which we may want to wait for,
 * or to kill outright, on unexpected termination).
 *
 * ap_spawn_child is a utility routine which handles an awful lot of
 * the rigamarole associated with spawning a child --- it arranges
 * for pipes to the child's stdin and stdout, if desired (if not,
 * set the associated args to NULL).  It takes as args a function
 * to call in the child, and an argument to be passed to the function.
 */

enum kill_conditions {
    kill_never,			/* process is never sent any signals */
    kill_always,		/* process is sent SIGKILL on pool cleanup */
    kill_after_timeout,		/* SIGTERM, wait 3 seconds, SIGKILL */
    just_wait,			/* wait forever for the process to complete */
    kill_only_once		/* send SIGTERM and then wait */
};

typedef struct child_info child_info;
API_EXPORT(void) ap_note_subprocess(pool *a, pid_t pid,
				    enum kill_conditions how);
API_EXPORT(int) ap_spawn_child(pool *, int (*)(void *, child_info *),
				   void *, enum kill_conditions,
				   FILE **pipe_in, FILE **pipe_out,
				   FILE **pipe_err);
int ap_close_fd_on_exec(int fd);

/* magic numbers --- min free bytes to consider a free pool block useable,
 * and the min amount to allocate if we have to go to malloc() */

#ifndef BLOCK_MINFREE
#define BLOCK_MINFREE 4096
#endif
#ifndef BLOCK_MINALLOC
#define BLOCK_MINALLOC 8192
#endif

/* Finally, some accounting */

API_EXPORT(long) ap_bytes_in_pool(pool *p);
API_EXPORT(long) ap_bytes_in_free_blocks(void);

#ifdef __cplusplus
}
#endif

#endif	/* !APACHE_ALLOC_H */
