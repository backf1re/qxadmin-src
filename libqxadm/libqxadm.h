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

#ifndef LIBQXADM_C
#define LIBQXADM_C

#ifdef WIN32

#ifdef LIBQXADM_EXPORTS
#define LIBQXADM_API __declspec(dllexport)
#else
#define LIBQXADM_API __declspec(dllimport)
#endif //LIBQXADM_EXPORTS
#include <winsock.h>

#else

#define LIBQXADM_API
#include <netinet/in.h> //sockaddr_in

#endif //WIN32


#define QUAKEWORLD 1
#define QUAKE2 2
#define QUAKE3 3
#define QW_DEF_PORT 27500
#define Q2_DEF_PORT 27910
#define Q3_DEF_PORT 27960

#define KEY_NAME_SIZE 32
#define KEY_VALUE_SIZE 64
#define KEYS_NUM 32

#define PLAYER_NAME_SIZE 32
#define PLAYERS_NUM 64
#define PASSWORD_SIZE 64

typedef struct {
		char name[KEY_NAME_SIZE];
		char value[KEY_VALUE_SIZE];
} key;

class LIBQXADM_API CQServer {

 private:

	enum { BUFFER_SIZE = 8192 };
 	int sock;						//socket fd
	int type;						//QUAKEWORLD,QUAKE2,QUAKE3
	struct sockaddr_in addr;
	char rcon_password[PASSWORD_SIZE];
 	unsigned int timeout;			//Default timeout: 2 seconds
 	char buffer[BUFFER_SIZE];
	
	char *send_recv(void);

	typedef struct {
		unsigned int id;
		int points;
		unsigned int ping;
		unsigned int address;
		char name[PLAYER_NAME_SIZE];
	} player;

 public:

	key keys[KEYS_NUM];					//server keys, set by get_keys();
	player players[PLAYERS_NUM];		//players, set by get_players();

	
	CQServer(int t = QUAKE3, char *address = "127.0.0.1", unsigned short port = Q3_DEF_PORT, char *password = "");
	~CQServer(void);

 	//address can be a DNS name or an IP(dotted form) address

	int set_type(int type);
	int set_address(char *address);
	void set_udp_port(unsigned short int udp_port);
	void set_rcon_password(char *rcon_password);
	void set_timeout(unsigned int timeout);
	
	int get_type(void);
	char *get_ip_address(void); //dotted form
	unsigned short int get_udp_port(void);
	char *get_rcon_password(void);
	unsigned int get_timeout(void);

	char *status(void);					//send a public status packet and return the reply
	char *rcon(char *command);			//send a remote console command and return the reply

	int get_keys(void);					//get keys from server and set "keys" variable
	char *get_key(char *key_name);		//return the value of a server key
	int get_players(void);				//get players from server and set "players" variable

};

#endif //LIBQXADM_C
