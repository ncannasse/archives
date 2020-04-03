/* OCaml-Win32
 * win32_system.c
 * Copyright (c) 2002 by Harry Chomsky & Nicolas Cannasse
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "../win32.h"

CAMLprim get_computer_name( value _ ) {
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
	char *buf = malloc(size);
	value s;
	if( !GetComputerName(buf,&size) ) {
		free(buf);
		raise_last_error();
	}
	s = copy_string(buf);
	free(buf);
	return s;
}

CAMLprim get_user_name( value _ ) {
	DWORD size = 128;
	char buf[128];
	if( !GetUserName(buf,&size) )
		raise_last_error();
	return copy_string(buf);
}
