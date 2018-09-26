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
	Disconnect immediately a client without a good key.[Thanks to Gag]
	Do a access() call on init instead of a stat() call, just for the fun of it
Version: 1.01 - 1999/07/04
	Changed encode and decode


Pierre-Andre Cote
pierreandre_cote@yahoo.com

*/

/* rlogviewd.c 1.01:

	arguments: rlogviewd [-p PORT] [-f LOGFILENAME] [-c CONFIGFILENAME] [-q] [-h]

	-q: "quiet": less output
	-h: print help text

	Defaults:	PORT = 55920
				LOGFILENAME = qconsole.log
				CONFIGFILENAME = rlogviewd.cfg

	config file options:	password = "PASSWORD"
							key = "HASHKEY": 8 hexadecimal digits
							port = "PORT"
							logfile = "LOGFILENAME"
							quiet = "0" or "1"

	command-line options override config file ones

TODO:	make case insensitive compare for config file options?
		buy a brand new UltraSparc server to test {en,de}code()

NOTE:	You should use "logfile 2" or 3 in quake2/3 but '1' could work too
		You can also simply redirect server output to a file(quake2 >> qlog.log)
		Do not seems to work in QW(at least in win98)...qwsv does not output anything
		even when redirected to a file...(???)

		In win32 file stats are updated only after the program(quake, etc) close the file.
		Even Explorer.exe shows the wrong size. FindFirstFile() is the function in Win32
		to obtain stats about a file, that's probably what Explorer uses because I have
		tried it and it returns the wrong size.
		The only way around this is to open the file and fstat it.
		That's what cygwin does. So that's what I use.
		This cause some problem if the program writing to the log file have opened it
		without shared read(?), rlogviewd will abort if it can't access the file with
		read permission. Why can we read a file open for write access? I don't know, but
		that really does suck.
		That's probably why QW does not output anything until quit. Q2/3 probably open
		the file, write it then close it.
*/

#include "rlogviewd.h"

//config or command-line parameters
unsigned short int server_port = 0;
char *password = 0;
unsigned int key = 0;
char *log_filename = 0;
char *cfg_filename = 0;					//config file name
int quiet = -1;							//reduce output if == 1

char *buffer;							//all purpose buffer
unsigned int log_size = 0;				//last stat log size

int main(int argc, char **argv) {

	int s;								//socket
	int size, retval, i, log_size_diff;
	struct sockaddr_in clientaddr;

	//read_fd_set: clients_fd_set + server socket
	//clients_fd_set: clients connected but can be not logged
	//logged_clients_fd_set: logged clients only
	fd_set read_fd_set, clients_fd_set, logged_clients_fd_set;
	struct timeval time, *time_ptr;
	int clients_num = 0;
	struct stat logstat;

#ifdef WIN32
	WIN32_FIND_DATA find_data;
	if(init_winsuck() == -1)
		return -1;
#endif

	printf("rlogviewd 1.01 (c)1999 Pierre-Andre Cote  'rlogviewd -h' for help.\n\n");

	//parse command line arguments
	if(parse_args(argc, argv) == -1)
		return -1;

	//parse config file
	if(parse_config_file(argv[0]) == -1)
		return -1;

	//validate parameters and set defaults
	if(validate_params() == -1)
		return -1;

	//check for log file read access
	if(access(log_filename, R_OK) == -1)
		error("rlogviewd: Can't read log file");

	//start the actual server
	if((s = make_server(server_port)) == 0) {
		fprintf(stderr, "rlogviewd: Error starting server. Exiting.\n");
		return -1;
	}

	printf("rlogviewd: Waiting for clients connection...\n");

	//tv_sec: timeout value for select() call
	//this effectively act like a sleep(tv_sec) call if there's no incoming data
	time.tv_sec = 1;
	time.tv_usec = 0;

	FD_ZERO(&clients_fd_set);
	FD_ZERO(&logged_clients_fd_set);
	buffer = (char *)malloc(BUFFER_SIZE);

	while(1) {

		if(clients_num < 1)
			time_ptr = NULL;		//there's no client connected, wait forever in select()
		else
			time_ptr = &time;

		if(stat(log_filename, &logstat) == -1)				//get log file size
			error("rlogviewd: Error stat'ing log file");

		log_size_diff = logstat.st_size - log_size;

		if(log_size_diff > 0 && clients_num > 0) {			//log has changed
			if(log_size_diff > MAX_TCP_DATA)
				log_size_diff = MAX_TCP_DATA;
			if((retval = read_log(log_size_diff)) != -1) {	//read log file
				for(i = 0; i < FD_SETSIZE; i++)				//send updated part to all clients
					if(FD_ISSET(i, &logged_clients_fd_set)) {
						//send log file update
						if((retval = send(i, buffer, retval, 0)) == -1) {
							close(i);
							FD_CLR((unsigned int)i, &clients_fd_set);
							FD_CLR((unsigned int)i, &logged_clients_fd_set);
							perror("rlogviewd: Error sending data to client(client connection closed)");
							clients_num--;
						} else
							if(!quiet)
								printf("rlogviewd: Sent something to client [%i] (%u bytes)\n", i, retval);
					}
			} else
				return -1;
		} else
			if(log_size_diff < 0) {
				printf("rlogviewd: Log file truncated! Will send next update.\n");
				log_size_diff = logstat.st_size - log_size;	//that will set log_size to logstat.st_size below
			}

		log_size += log_size_diff;							//log_size_diff is modified if > MAX_TCP_DATA

#ifdef DEBUG
		printf("DEBUG: log_size = %u\n", log_size);
#endif

		/* Block until input arrives on one or more active sockets or timeout occurs */
		read_fd_set = clients_fd_set;
		FD_SET(s, &read_fd_set);
		if((retval = select(FD_SETSIZE, &read_fd_set, NULL, NULL, time_ptr)) < 0)
			error("rlogviewd: select");
		if(retval == 0)	//timeout occured
			continue;

		/* Service all the sockets with input pending. */
		for(i = 0; i < FD_SETSIZE; ++i)
			if(FD_ISSET(i, &read_fd_set)) {
				if(i == s) {
					/* Connection request on original socket. */
					size = sizeof(clientaddr);
					if((retval = accept(s, (struct sockaddr *)&clientaddr, &size)) < 0)
						perror("rlogviewd: accept");
					else {
						printf("rlogviewd: Connect from host %s:%hd [%i].\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retval);
						FD_SET(retval, &clients_fd_set);
						clients_num++;
					}
				} else {
					/* Data arriving on an already-connected socket. */
					retval = recv(i, buffer, BUFFER_SIZE, 0);
					if(retval <= 0) {
						close(i);
						FD_CLR((unsigned int)i, &clients_fd_set);
						FD_CLR((unsigned int)i, &logged_clients_fd_set);
						printf("rlogviewd: Client [%d] disconnected.\n", i);
						clients_num--;
					} else {
						//decode message
						decode(buffer, retval);
						if(!quiet)
							printf("rlogviewd: Got message: '%s'\n", buffer);
						//check for password
						if(*((unsigned int *)buffer) == ntohl(PASS)) {		//password message
							if(!strcmp(buffer+5, password)) {
								sprintf(buffer, "YIPI %s", log_filename);	//good password
								retval = strlen(buffer);
								encode(buffer, retval);
								send(i, buffer, retval, 0);
								printf("rlogviewd: Client [%d] successfully logged in.\n", i);
								FD_SET((unsigned int)i, &clients_fd_set);
								FD_SET((unsigned int)i, &logged_clients_fd_set);
							} else {										//wrong password
								printf("rlogviewd: Wrong Password! [%d]\n", i);
								strcpy(buffer, "WO!!");
								retval = strlen(buffer);
								encode(buffer, retval);
								send(i, buffer, retval, 0);
								close(i);
								FD_CLR((unsigned int)i, &clients_fd_set);
								clients_num--;
							}
						} else {	//wrong key, probably
							printf("rlogviewd: Client [%i] can't login, probably wrong key(client connection closed).\n", i);
							//send back its crap
							encode(buffer, retval);
							send(i, buffer, retval, 0);
							close(i);
							FD_CLR((unsigned int)i, &clients_fd_set);
						}
					}
				}
			}
	}

}

//read log_size_diff bytes at end of log file
// encode them and returns size of buffer(number of encoded bytes)
int read_log(int log_size_diff) {

	int numbytes, log;

	//open file and read the updated part
	if((log = open(log_filename, O_RDONLY | O_BINARY)) == -1)
		error("rlogviewd: Error opening log file");
	if(lseek(log, log_size, SEEK_SET) == -1)
		error("rlogviewd: Error lseek'ing log file");
	if((numbytes = read(log, buffer, log_size_diff)) <= 0)
		error("rlogviewd: Error reading log file");
	if(numbytes != log_size_diff)
		printf("rlogviewd: log size diff == %d but read %d bytes. Continuing...\n", log_size_diff, numbytes);

	close(log);

	//encode new log text
	encode(buffer, numbytes);
	return numbytes;
}

//encode outgoing data
char *encode(char *data, unsigned long int datalen) {

	unsigned long int i;
	char *key_p = (char *)&key;

	//encode data
	for(i = 0; i < datalen; i++)
		data[i] ^= key_p[i % 4];

	data[datalen] = 0;	//null terminate(?)

	return data;
}

//decode incoming data
char *decode(char *data, unsigned long int datalen) {
	return encode(data, datalen);
}

//parse arguments
int parse_args(int argc, char **argv) {

	int i;
	for(i = 1; i < argc; i++) {
		if(*argv[i] == '-')
			switch(tolower(*(argv[i] + 1))) {
				case 'p':
					server_port = atoi(argv[++i]);
					break;
				case 'f':
					log_filename = strdup(argv[++i]);
					break;
				case 'c':
					cfg_filename = strdup(argv[++i]);
					break;
				case 'q':
					quiet = 1;
					break;
				case 'h':
					print_help();
					return -1;
				default:
					printf("rlogviewd: Unknown command-line option '%c' ignored.\n", *(argv[i] + 1));
			}
	}
	return 0;
}

int parse_config_file(char *exe_filename) {

	FILE *cfg_file;
	char param[128];
	char value[256];
	char dummy[16];

	if(!cfg_filename) {				//no config filename on commandline, use default
		cfg_filename = (char *)malloc(strlen(exe_filename) + strlen(CFG_DEF_NAME));
		//keep only path
		exe_filename[(int)strrchr(exe_filename, PATH_SEPARATOR) - (int)exe_filename + 1] = 0;
		if(*exe_filename == '"')		//cygwin adds " around exe name, get rid of it
			exe_filename++;
		strcpy(cfg_filename, exe_filename);
		if(!cfg_filename) {
			printf("rlogviewd: Something's wrong. Exiting.\n");
			return -1;
		}
		strcat(cfg_filename, CFG_DEF_NAME);
	}
	if((cfg_file = fopen(cfg_filename, "r")) == 0) {
		sprintf(value, "rlogviewd: Error opening config file '%s'", cfg_filename);
		error(value);
	}

	free(cfg_filename);

	//parse parameters from config file
	while(fscanf(cfg_file, "%127[^= ]%15[ =]\"%255[^\"\n]\"\n", &param, &dummy, &value) == 3) {
		if(!strcmp("password", param)) {
			password = strdup(value);
			continue;
		}
		if(!strcmp("key", param)) {
			key = htonl(strtoul(value, NULL, 16));
			continue;
		}
		if(server_port == 0 && !strcmp("port", param)) {
			server_port = atoi(value);
			continue;
		}
		if(log_filename == 0 && !strcmp("logfile", param)) {
			log_filename = strdup(value);
			continue;
		}
		if(quiet == -1 && !strcmp("quiet", param)) {
			quiet = atoi(value);
			continue;
		}
	}

	fclose(cfg_file);

	return 0;
}

//validate parameters and set defaults
int validate_params() {

	//check for empty password
	if(password == 0) {
		printf("rlogviewd: You have to set a password in your config file!");
		printf("rlogviewd: Add a line like this in it: password = \"mYpaSsWORd\"");
		return -1;
	}
	if(key == 0) {
		printf("rlogviewd: Key is invalid! Verify it in your config file.\n");
		printf("rlogviewd: Key must be an 8 digits hexadecimal number(eg.: f452a9cb).\n");
		return -1;
	}

	if(quiet == -1)
		quiet = 0;

	if(server_port == 0) {
		printf("rlogviewd: No port specified on command-line or in config file, using default(%u).\n", SERVER_DEF_PORT);
		server_port = SERVER_DEF_PORT;
	}
	if(log_filename == 0) {
		printf("rlogviewd: No log filename specified on command-line or in config file, using default(qconsole.log).\n");
		log_filename = strdup("qconsole.log");
	}
	printf("rlogviewd: Listening to port %u\n", server_port);
	printf("rlogviewd: Log file: '%s'\n", log_filename);

	return 0;
}

//bind a server to 'port' and return socket on success(0 on failure)
int make_server(unsigned short int port) {

	int s;
	struct sockaddr_in saddr;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if(s < 0) {
		perror("rlogviewd: socket");
		return 0;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	if(bind(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("rlogviewd: bind");
		return 0;
	}
	if(listen(s, 1) < 0) {
		perror("rlogviewd: listen");
		return 0;
	}

	return s;
}

void print_help() {

	const char *help_text = "Usage: rlogviewd [-p PORT] [-f LOGFILENAME] [-c CONFIGFILENAME] [-q] [-h]\n\n\
-q: \"quiet\": less output\n\
-h: this text\n\n\
Defaults: PORT = 55920\n\
          LOGFILENAME = qconsole.log\n\
          CONFIGFILENAME = rlogviewd.cfg\n\n\
config file options: port = \"PORT\"\n\
                     password = \"PASSWORD\"\n\
                     logfile = \"LOGFILENAME\"\n\
                     quiet = \"0\" or \"1\"\n\
                     key = \"HASHKEY\": 8 hexadecimal digits\n\n\
command-line options override config file ones\n\n";

	printf(help_text);
}

#ifdef WIN32
int init_winsuck() {
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(1,1), &WSAData)) {
//		MessageBox(NULL, "Error Initializing WinSock.", "rlogviewd", MB_OK);
		printf("Error Initializing WinSock.");
		return -1;
	}
	return 0;
}
#endif


/*

new encode/decode funtions, not used

//encode outgoing data
//data can grow by at most 9 bytes(at least 5 byte)
//
//last byte contains the number of padding bytes + 1
//NEW ALGORYTHM: firt 4 bytes is length of data(excluding length...)
//[to prevent condition where TCP joins 2 or more packets]
unsigned long int encode(char *data, unsigned long int datalen) {

	unsigned long int i, ndatalen, pad_num;	//pad_num: number of padding bytes(0-3)
	unsigned long int *datai = (unsigned long int *)data;
	unsigned long int mod =  datalen % 4;

	//move data 4 bytes down
	memmove(data+4, data, datalen);

	//calculate number of padding bytes
	pad_num = mod == 0 ? 0 : 4 - mod;

	//put data length in the first 4 bytes of data(in network order)
	ndatalen = htonl(datalen + pad_num);
	memcpy(data, (unsigned long int *)&ndatalen, 4);
	datalen += 4;

	//append padding
	memset(data + datalen, '\xff', pad_num);
	datalen += pad_num;

	//encode data
	for(i = 1; i < datalen / 4; i++)
		datai[i] ^= key;

	//append number of padding bytes + 1
	memset(data + datalen++, pad_num+1, 1);
	memset(data + datalen, 0, 1);	//null terminated(?)

	printf("DEBUG: datalen = %u, mod 4 = %u\n", datalen, datalen % 4);

	return datalen;
}

//decode incoming data
//last byte contains the number of padding bytes + 1
char *decode(char *data, unsigned long int datalen) {

	unsigned long int i, blocklen;
	unsigned long int *datai = (unsigned long int *)data;

	blocklen = ntohl(*datai);

	memmove(data, data+4, blocklen+1);

	printf("DEBUG: blocklen = %u, datalen = %u\n", blocklen, datalen);

	if(blocklen % 4 != 0)
		printf("rlogviewd: Error decoding data! Attempting to continue...\n");
	//decode data
	for(i = 0; i < blocklen / 4; i++)
		datai[i] ^= key;

	//remove padding
	data[blocklen - data[blocklen] + 1] = 0;

	if(blocklen + 5 < datalen)
		printf("There's is more data...Not supposed to happen\n");

	return data;
}

*/
