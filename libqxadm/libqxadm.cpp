/*
QxAdmin: remotely controls QuakeWorld, Quake 2, Quake 3: Arena
         servers from a Win32 machine.

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

Version: 0.9 - 1999/05/06 - Initial release.
Version: 1.0 - 1999/05/23
	Always default port check box
	Always console variables check box
	Server infos scrollbar memory
	Right-click pop-up menu to kick player
	Q3:A colors control characters are removed
	Server port Spin control
Version: 1.01 - 1999/05/26
	Auto-update
Version: 1.02 - 1999/05/26
	auto-update spin disable bug
	disable cvars button in QW
Version: 1.03 - 1999/06/10
	repaired kick right-click menu command

Pierre-Andre Cote
pierreandre_cote@yahoo.com

*/

#include "libqxadm.h"
#include <stdio.h>		//snprintf
#include <stdlib.h>		//qsort
#include <string.h>		//memset

#ifndef WIN32
#include <arpa/inet.h>	//inet_addr
#include <netinet/in.h>	//sockaddr_in, htons
#include <netdb.h>		//gethostbyname
#include <sys/socket.h> //AF_INET, ....
#include <sys/time.h>	//timeval,...
#include <unistd.h>		//close
#endif WIN32

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif //WIN32

//CQServer::CQServer(void) {

//}

CQServer::CQServer(int t, char *address, unsigned short int port, char *password) {

#ifdef WIN32
	//init winsock... this blows.
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);
#endif
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1) {
		type = 0;
		return;
	}
	
	timeout = 2;		//default timeout: 2 seconds
	
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	set_type(t);
	set_address(address);
	set_udp_port(port);
	set_rcon_password(password);

	keys[0].name[0] = 0;
	keys[0].value[0] = 0;
}

CQServer::~CQServer(void) {
#ifndef WIN32
	close(sock);
#else
	closesocket(sock);
#endif
}

int CQServer::set_type(int t) {
	if(t >= QUAKEWORLD && t <= QUAKE3 || t == 0) {
		type = t;
		return 0;
	} else
		return -1;
}

int CQServer::set_address(char *address) {
	addr.sin_addr.s_addr = inet_addr(address);
	if((int)addr.sin_addr.s_addr == -1) {	//invalid address, could be a DNS name
		//DNS query
		struct hostent *dnsentry = gethostbyname(address);
		if(dnsentry == 0)	//not a DNS name
			return -1;
		else
			addr.sin_addr = *(struct in_addr *)dnsentry->h_addr;
	}
	return 0;
}

void CQServer::set_udp_port(unsigned short int port) {
	addr.sin_port = htons(port);
}

void CQServer::set_rcon_password(char *password) {
	strncpy(rcon_password, password, PASSWORD_SIZE);
}

void CQServer::set_timeout(unsigned int to) {
	timeout = to;
}

int CQServer::get_type(void) {
	return type;
}

char *CQServer::get_ip_address(void) {
	return inet_ntoa(addr.sin_addr);
}

unsigned short int CQServer::get_udp_port(void) {
	return ntohs(addr.sin_port);
}

char *CQServer::get_rcon_password(void) {
	return (char *)rcon_password;	
}

unsigned int CQServer::get_timeout(void) {
	return timeout;	
}

char *CQServer::send_recv(void) {

	int retval, buflen, gotone = 0;
	struct timeval time;
	fd_set set;
	char buffer2[BUFFER_SIZE];
	char *buf_ptr;

	buflen = strlen(buffer)+1;				//strlen+1: send a null terminated string
	retval = sendto(sock, buffer, buflen, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
	if(retval == -1 || retval != buflen)
		return 0;

	buffer[0] = (char)0;
	buffer2[0] = (char)0;

	time.tv_sec = timeout;
	time.tv_usec = 0;
	FD_ZERO(&set);
	FD_SET(sock, &set);
	
	//wait for reply...for "timeout" seconds
	//get all packets: Q2 send long replies in multiple packets...(maybe Q3...not 1.03)
	do {
		retval = select(FD_SETSIZE, &set, NULL, NULL, &time);
		if(retval == 0 || retval == -1)
			if(buffer2[0] != 0)
				return buffer;			//the reply fitted in 1 packet
			else
				return 0;

		retval = recv(sock, buffer2, BUFFER_SIZE, 0);
		if(retval == 0 || retval == -1)
			if(buffer2[0] != 0)
				return buffer;			//not supposed to happen here, see above
			else
				return 0;
		buffer2[retval] = (char)0;
		if(get_type() == QUAKE2 && gotone)		//remove \xff\xff\xff\xffprint\n between packets
			buf_ptr = buffer2 + 10;
		else
			buf_ptr = buffer2;
		
		gotone = 1;

		strncat(buffer, buf_ptr, BUFFER_SIZE - 1);
		time.tv_sec = 0;
		time.tv_usec = 10;				//wait for 1 usec for the 2nd packet(too fast?)
	} while(retval != 0);

	return buffer;
}

char *CQServer::status(void) {
	
	switch(type) {
		case QUAKE3:
			strcpy(buffer, "\xff\xff\xff\xffgetstatus"); break;
		case QUAKE2:
		case QUAKEWORLD:
			strcpy(buffer, "\xff\xff\xff\xffstatus"); break;
		default: return 0;
	}

	return send_recv();
}

char *CQServer::rcon(char *command) {

#ifdef WIN32
	_snprintf(buffer, BUFFER_SIZE, "\xff\xff\xff\xffrcon %s %s", rcon_password, command);
#else
	snprintf(buffer, BUFFER_SIZE, "\xff\xff\xff\xffrcon %s %s", rcon_password, command);
#endif

	return send_recv();
}

int compare_keys(const void *a, const void *b) {
#ifdef WIN32
	return stricmp(((key *)a)->name, ((key *)b)->name);
#else
	return strcasecmp(((key *)a)->name, ((key *)b)->name);
#endif
}

int CQServer::get_keys(void) {

	char keystring[3072];
	char *tmp;
	int i;

	if(status() == 0)
		return -1;

	strncpy(keystring, buffer, 3072);

	tmp = strchr(keystring,'\\');

	tmp = strtok(tmp, "\\");
	for(i=0; tmp != 0 && i < KEYS_NUM-1; i++, tmp = strtok(0,"\\")) {
		strncpy(keys[i].name, tmp, KEY_NAME_SIZE);
		if((tmp = strtok(0, "\\"))) {
			strncpy(keys[i].value, tmp, KEY_VALUE_SIZE);
		}
	}
	strtok(keys[i-1].value,"\n");					//remove 0xa from last valid key value
	keys[i].name[0] = 0;
	keys[i].value[0] = 0;

	qsort(keys, i, sizeof(key), compare_keys);

	return 0;
}

int CQServer::get_players(void) {

	int points, dummyint;
	unsigned int ping, id;
	int i;
	char name[32];

	if(status() == 0) return -1;

	char *token = strtok(buffer, "\n");				//skip type of response, server keys
	if(get_type() != QUAKEWORLD)
		token = strtok(NULL, "\n");

	for(i=0; (token = strtok(NULL, "\n")) != 0; i++) {
		if(get_type() != QUAKEWORLD)
			sscanf(token, "%d %d \"%32[^\"]\"", &points, &ping, &name);
		else {
			sscanf(token, "%d %d %d %d \"%32[^\"]\"", &id, &points, &dummyint/*time*/, &ping, &name);
			players[i].id = id;
		}
		players[i].points = points;
		players[i].ping = ping;
		strncpy(players[i].name, name, 32);
		//add_player(0/*id*/, name, point, ping, 0/*address*/);
	}
	players[i].name[0] = 0;

	return 0;
}

char *CQServer::get_key(char *key_name) {

	for(int i=0 ; (keys[i].name[0] != 0) && (i < KEYS_NUM) ; i++) {
#ifdef WIN32
		if(stricmp(keys[i].name, key_name) == 0)
#else
		if(strcasecmp(keys[i].name, key_name) == 0)
#endif
			return keys[i].value;
	}

	return 0;
}
