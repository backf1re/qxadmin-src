/*
rlogviewd: send log files updates to rlogview clients.

Copyright (C) 1999  Pierre-Andre Cote

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Version: 0.98 - 1999/06/21 - Initial release
Version: 0.99 - 1999/06/23
	Fixed logfile being sent in clear text when connecting with a telnet client.
	[Thanks to Gag for pointing this out]
Version: 1.0beta1 - 1999/06/28
	Correctly handle big files at start-up
	Maxed update packet size to 8K
	Changed packet encoding then reversed the changes
	Added logged_clients_fd_set: non logged clients could receive updates...
Version: 1.0 - 1999/06/30
	Disconnect immediately a client without a good key
	Do a access() call on init instead of a stat() call, just for the fun it

Pierre-Andre Cote
pierreandre_cote@yahoo.com

*/
/* rlogviewd.h

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>

#ifndef WIN32

#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PATH_SEPARATOR '/'

#else

#include <io.h>
#include <winsock.h>

#define close(blah) closesocket(blah)
#define PATH_SEPARATOR '\\'

#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif
#define error(_err_msg) { perror(_err_msg); /*getchar();*/ return -1; }

#define SERVER_DEF_PORT 55920	//27960*2
#define BUFFER_SIZE 65536
#define MAX_TCP_DATA 8192		//max sent packet size will be a little larger than this
#define PASS ('P'<<24)+('A'<<16)+('S'<<8)+'S'
#define CFG_DEF_NAME "rlogviewd.cfg"

//#define FD_SETSIZE      256

//function prototypes
void print_help();
int parse_args(int argc, char **argv);
int parse_config_file(char *exe_filename);
int validate_params();
int make_server(unsigned short int port);

char *encode(char *msg, unsigned long int datalen);
char *decode(char *msg, unsigned long int datalen);

int read_log(int log_size_diff);

#ifdef WIN32
int init_winsuck();
#endif
