
/***************************************************************************
 *   Copyright (C) 2004 by Willy Gardiol                                   *
 *   willy@gardiol.org                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "wpd.h"

#include "wpd_commands.h"

#define HOST "localhost"

/* The following functions are not static because are used by wpd_main.c 
 * But SHOULD not be used by clients, go directly to the real public interface */
int atom_send( int tmp_socket, char* buf, int len, ERROR* err )
{
	int l = 0;
	int ret = 0;
	while ( l < len )
	{
		ret = send( tmp_socket, &buf[l], len - l, MSG_NOSIGNAL );
		if ( ret < 0 )
		{
			perror("atom_send: ");
			close(tmp_socket);
			err[0] = WRITE_ERROR;
			return -1;
		}
		else if ( ret == 0 )
		{
			err[0] = WRITE_ERROR;
			return -1;
		}
		l += ret;
	}
	return 0;
}

int atom_read( int tmp_socket, char* buf, int len, ERROR* err )
{
	int l = 0;
	int ret = 0;
	while ( l < len )
	{
		ret = recv( tmp_socket, &buf[l], len - l, MSG_NOSIGNAL );
		if ( ret < 0 )
		{
			perror("atom_read: ");
			close( tmp_socket );
			err[0] = READ_ERROR;
			return -1;
		}
		else if ( ret == 0 )
		{
			err[0] = READ_ERROR;
			return -1;
		}
		l += ret;
	}
	return 0;
}

static int wpd_connect(int port, ERROR* err)
{
   char hostname[100];
	int	tmp_socket;
	struct sockaddr_in pin;
	struct hostent *hp;

   strcpy(hostname,HOST);

	if ((hp = gethostbyname(hostname)) == 0) {
		perror("gethostbyname: ");
		err[0] = CONNECT_ERROR;
		return -1;
	}

	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(port);

	if ((tmp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("tmp_socket");
		err[0] = CONNECT_ERROR;
		return -1;
	}

	/* connect to PORT on HOST */
	if (connect(tmp_socket,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		perror("connect");
		close(tmp_socket);
		err[0] = CONNECT_ERROR;
		return -1;
	}

	return tmp_socket;
}

static int send_and_receive( int tmp_socket, int cmm, char* buffer, ERROR* err )
{
	int len = COMMAND_LENGTH;

	if ( tmp_socket == -1 )
	{
		err[0] = READ_ERROR;
		return -1;
	}
	
	if ( atom_send( tmp_socket, (char*)&len, sizeof(int), err ) == -1 )
	{
		return -1;
	}
	if ( atom_send( tmp_socket, (char*)&cmm, len, err ) == -1 )
	{
		return -1;
	}
	if ( atom_read( tmp_socket, (char*)&len, sizeof(int), err ) == -1 )
		return -1;
	
	if ( atom_read( tmp_socket, buffer, len, err ) == -1 )
		return -1;

	return len;
}

/*
 *
 *
 *
 *
 *
 *
 */

static int port = 1978;
void wpd_setup(int p)
{
	port = p;
}

int send_command_str( int comm, char * str, int tmp_socket )
{
	int len;
	ERROR err = NO_ERROR;
	int close_socket = 0;
	
	if ( tmp_socket == -1 )
	{
		tmp_socket = wpd_connect( port, &err );
		if ( err != NO_ERROR )
			return -1;
		close_socket = 1;
	}

	len = send_and_receive( tmp_socket,comm, str, &err );
	if ( err != NO_ERROR ) {
		if ( close_socket )
			close( tmp_socket );
		return -1;
	}

	if ( close_socket )
		close( tmp_socket );
	
	str[len] = '\0';
	
	return strlen(str);
}

int send_command_array( int comm, int * array, int tmp_socket )
{
	int len;
	ERROR err = NO_ERROR;
	int close_socket = 0;
	
	if ( tmp_socket == -1 )
	{
		tmp_socket = wpd_connect( port, &err );
		if ( err != NO_ERROR )
			return -1;
		close_socket = 1;
	}

	len = send_and_receive( tmp_socket,comm, (char*)array, &err );
	if ( err != NO_ERROR ) {
		if ( close_socket )
			close( tmp_socket );
		return -1;
	}

	if ( close_socket )
		close( tmp_socket );
	
	return len;
}

int send_command( int comm, int tmp_socket, int * error )
{
	int len, ret;
	ERROR err = NO_ERROR;
	int close_socket = 0;
	error[0] = 0;
	
	if ( tmp_socket == -1 )
	{
		tmp_socket = wpd_connect( port, &err );
		if ( err != NO_ERROR )
		{
			error[0] = 1;
			return -1;
		}
		close_socket = 1;
	}
		
	len = send_and_receive( tmp_socket,comm, (char*)&ret, &err );
	if ( err != NO_ERROR ) {
		error[0] = 1;
		return -1;
	}
	
	if ( close_socket )
		close( tmp_socket );

	return ret;
}

int setup_connection()
{
	int len = 0, ret;
	ERROR err = NO_ERROR;
	int comm = COMMAND_KEEP_CONNECTION;
	int tmp_socket = wpd_connect( port, &err );

	if ( err != NO_ERROR )
		return -1;
	
	len = send_and_receive( tmp_socket, comm, (char*)&ret, &err );
	if ( err != NO_ERROR ) {
		return -1;
	}
	
	if ( ret == -1 )
	{
		close(tmp_socket);
		return -1;
	}

	return tmp_socket;
}



/* this is not public */
static void setup_keep_connection( int* tmp_socket )
{
	tmp_socket[0] = setup_connection();
	if ( tmp_socket[0] == -1 )
		printf("Failed, falling back to one shoot connection (slow).\n");
	else
		printf("Success, socket is n. %d\n", tmp_socket[0] );
}



int keep_send_command( int cmm, int* tmp_socket )
{
	int ret = -1;
	int err = 0;
	if ( tmp_socket[0] == -1 )
		setup_keep_connection( tmp_socket );

	ret = send_command( cmm, tmp_socket[0], &err );
	if ( err == 1 )
		tmp_socket[0] = -1;
	return ret;
}

int keep_send_command_str( int cmm, char* str, int* tmp_socket )
{
	int ret = 0;
	if ( tmp_socket[0] == -1 )
		setup_keep_connection( tmp_socket );

	ret = send_command_str( cmm, str, tmp_socket[0] );
	if ( ret == -1 )
	{
		ret = 0;
		tmp_socket[0] = -1;
	}
	return ret;
}

int keep_send_command_array( int cmm, int* str, int* tmp_socket )
{
	int ret = 0;
	if ( tmp_socket[0] == -1 )
		setup_keep_connection( tmp_socket );

	ret = send_command_array( cmm, str, tmp_socket[0] );
	if ( ret == -1 )
	{
		ret = 0;
		tmp_socket[0] = -1;
	}
	return ret;
}


