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
#if 0
	
	if(argc == 1)
	{
		throw LinuxPowerUSBError( "%s: missing operands\n", argv[0]);
	}
	else if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0)
		{
			usage(argv);
			return 1;
		}
		else
		{
			int p = atoi( argv[1] );
			lp.reportStatus(lp.getPortState(p), p);
		}
	}
	else if(argc == 3)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p = atoi( argv[2] );
			lp.reportDefault(lp.getPortDefaultState(p), p);
		}
		else
		{
			int p = atoi(argv[1]);
			int how = strcmp( argv[2], "on" ) == 0;
		}
	}
	else if(argc == 4)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p = atoi( argv[2] );
			int how = strcmp( argv[3], "on" ) == 0;
			lp.setPortDefaultState( p, how );
		}
		else
		{
			throw LinuxPowerUSBError("Invalid Argument(s): Try `%s --help` for more information.", argv[0]);	
		}
	}
	else
	{
		throw LinuxPowerUSBError("Incorrect Usage: Try `%s --help` for more information.", argv[0]);
	}
#endif
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
