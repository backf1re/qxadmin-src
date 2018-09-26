// libqxadm library test program
//compile with: gcc -c libqxadm.cpp && gcc test.cpp libqxadm.o -o test

#include "libqxadm.h"
#include <stdio.h>

int main(int argc, char **argv) {

	CQServer q3l(QUAKE3, "localhost", 0, "rconpassword");

	printf("<status>\n");
	printf(q3l.status());
	printf("<END>\n");

	if(q3l.get_keys())
		printf("Current map name: %s\n", q3l.get_key("mapname"));
	
	printf("<rcon status>\n");
	printf(q3l.rcon("status"));
	printf("<END>\n");
	
}
