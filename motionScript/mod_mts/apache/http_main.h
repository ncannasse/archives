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

#ifndef APACHE_HTTP_MAIN_H
#define APACHE_HTTP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Routines in http_main.c which other code --- in particular modules ---
 * may want to call.  Right now, that's limited to timeout handling.
 * There are two functions which modules can call to trigger a timeout
 * (with the per-virtual-server timeout duration); these are hard_timeout
 * and soft_timeout.
 *
 * The difference between the two is what happens when the timeout
 * expires (or earlier than that, if the client connection aborts) ---
 * a soft_timeout just puts the connection to the client in an
 * "aborted" state, which will cause http_protocol.c to stop trying to
 * talk to the client, but otherwise allows the code to continue normally.
 * hard_timeout(), by contrast, logs the request, and then aborts it
 * completely --- longjmp()ing out to the accept() loop in http_main.
 * Any resources tied into the request's resource pool will be cleaned up;
 * everything that isn't will leak.
 *
 * soft_timeout() is recommended as a general rule, because it gives your
 * code a chance to clean up.  However, hard_timeout() may be the most
 * convenient way of dealing with timeouts waiting for some external
 * resource other than the client, if you can live with the restrictions.
 *
 * (When a hard timeout is in scope, critical sections can be guarded
 * with block_alarms() and unblock_alarms() --- these are declared in
 * alloc.c because they are most often used in conjunction with
 * routines to allocate something or other, to make sure that the
 * cleanup does get registered before any alarm is allowed to happen
 * which might require it to be cleaned up; they * are, however,
 * implemented in http_main.c).
 *
 * NOTE!  It's not "fair" for a hard_timeout to be in scope through calls
 * across modules.  Your module code really has no idea what other modules may
 * be present in the server, and they may not take too kindly to having a
 * longjmp() happen -- it could result in corrupted state.  Heck they may not
 * even take to kindly to a soft_timeout()... because it can cause EINTR to
 * happen on pretty much any syscall, and unless all the libraries and modules
 * in use are known to deal well with EINTR it could cause corruption as well.
 * But things are likely to do much better with a soft_timeout in scope than a
 * hard_timeout.
 * 
 * A module MAY NOT use a hard_timeout() across * sub_req_lookup_xxx()
 * functions, or across run_sub_request() functions.  A module SHOULD NOT use a
 * soft_timeout() in either of these cases, but sometimes there's just no
 * choice.
 *
 * kill_timeout() will disarm either variety of timeout.
 *
 * reset_timeout() resets the timeout in progress.
 */

API_EXPORT(void) ap_start_shutdown(void);
API_EXPORT(void) ap_start_restart(int);
API_EXPORT(void) ap_hard_timeout(char *, request_rec *);
API_EXPORT(void) ap_keepalive_timeout(char *, request_rec *);
API_EXPORT(void) ap_soft_timeout(char *, request_rec *);
API_EXPORT(void) ap_kill_timeout(request_rec *);
API_EXPORT(void) ap_reset_timeout(request_rec *);

API_EXPORT(void) ap_child_terminate(request_rec *r);
API_EXPORT(void) ap_sync_scoreboard_image(void);
API_EXPORT(int) ap_update_child_status(int child_num, int status, request_rec *r);
void ap_time_process_request(int child_num, int status);
API_EXPORT(unsigned int) ap_set_callback_and_alarm(void (*fn) (int), int x);
API_EXPORT(int) ap_check_alarm(void);

void setup_signal_names(char *prefix);

/* functions for determination and setting of accept() mutexing */
char *ap_default_mutex_method(void);
char *ap_init_mutex_method(char *t);

#ifndef NO_OTHER_CHILD
/*
 * register an other_child -- a child which the main loop keeps track of
 * and knows it is different than the rest of the scoreboard.
 *
 * pid is the pid of the child.
 *
 * maintenance is a function that is invoked with a reason, the data
 * pointer passed here, and when appropriate a status result from waitpid().
 *
 * write_fd is an fd that is probed for writing by select() if it is ever
 * unwritable, then maintenance is invoked with reason OC_REASON_UNWRITABLE.
 * This is useful for log pipe children, to know when they've blocked.  To
 * disable this feature, use -1 for write_fd.
 */
API_EXPORT(void) ap_register_other_child(int pid,
       void (*maintenance) (int reason, void *data, ap_wait_t status), void *data,
				      int write_fd);
#define OC_REASON_DEATH		0	/* child has died, caller must call
					 * unregister still */
#define OC_REASON_UNWRITABLE	1	/* write_fd is unwritable */
#define OC_REASON_RESTART	2	/* a restart is occuring, perform
					 * any necessary cleanup (including
					 * sending a special signal to child)
					 */
#define OC_REASON_UNREGISTER	3	/* unregister has been called, do
					 * whatever is necessary (including
					 * kill the child) */
#define OC_REASON_LOST		4	/* somehow the child exited without
					 * us knowing ... buggy os? */

/*
 * unregister an other_child.  Note that the data pointer is used here, and
 * is assumed to be unique per other_child.  This is because the pid and
 * write_fd are possibly killed off separately.
 */
API_EXPORT(void) ap_unregister_other_child(void *data);

#endif

#ifdef __cplusplus
}
#endif

#endif	/* !APACHE_HTTP_MAIN_H */
