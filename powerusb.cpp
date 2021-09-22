#include "powerusb.h"
#include "LinuxPowerUSB.hpp"

void usage(char **argv) {
	fprintf( stderr, "Usage: %s [-dev] -p port [-s state]\n", argv[0]);
}

int handleRequest(LinuxPowerUSB &lp, int argc, char* argv[])
{
	bool verbose = false;
	bool defState = false;
	bool useexit = false;
	int port = 0;
	char *state = NULL;
	int c;
	while( (c = getopt(argc, argv, "s:p:dev")) != -1 ) {
		switch( c ) {
			case 'v': verbose = true; break;
			case 'd': defState = true; break;
			case 'p': port = atoi(optarg); break;
			case 's': state = strdup(optarg); break;
			case 'e': useexit = true; break;
			case -1: break;
			default:
				throw LinuxPowerUSBError( "bad argument: '%d'", c);
		}
	}

	if( port == 0 ) {
		throw LinuxPowerUSBError( "missing port #: -p N required\n");
	}

	bool how = false;
	if( state != NULL ) {
		how = strcasecmp( state, "on") == 0;
	}

	if( defState ) {
		if( state == NULL )
			if( useexit ) return(lp.getPortDefaultState(port));
			else lp.reportDefault(lp.getPortDefaultState(port), port, verbose);
		else
			lp.setPortDefaultState( port, how );

	} else {
		if( state == NULL )
			if( useexit ) return(lp.getPortState(port));
			else lp.reportStatus(lp.getPortState(port), port, verbose);
		else
			lp.setPortState( port, how );
	}
	return 0;
}



int main(int argc, char* argv[])
{
	LinuxPowerUSB p;
	try {
		p.setDebug(false);
		p.Setup();
		return handleRequest(p, argc, argv);
	} catch( LinuxPowerUSBError &ex ) {
		p.error( "%s: %s\n", argv[0], ex.what() );
	}
	return 2;
}
