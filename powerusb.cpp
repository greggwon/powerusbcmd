#include "powerusb.h"
#include "LinuxPowerUSB.hpp"

void usage(char **argv) {
	fprintf( stderr, "Usage:\n");
	fprintf( stderr, "  1) %s portnumber state       ---Sets the state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3, state = on/off\n");			
	fprintf( stderr, "     Ex: powerusb 2 on               ---Switches on Port 2---\n");
	fprintf( stderr, "  2) %s -d portnumber state    ---Sets the default state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3, state = on/off\n");
	fprintf( stderr, "     Ex: powerusb -d 2 on            ---Sets the default state of Port 2 to on---\n");			
	fprintf( stderr, "  3) %s portnumber             ---Reads the state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3\n");
	fprintf( stderr, "     Ex: powerusb 2                  ---Returns the state of Port 2---\n");
	fprintf( stderr, "  4) %s -d portnumber          ---Reads the default state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3\n");
	fprintf( stderr, "     Ex: powerusb -d 2               ---Returns the default state of Port 2---\n");
}

int handleRequest(LinuxPowerUSB &lp, int argc, char* argv[])
{
	if(argc == 1)
	{
		throw LinuxPowerUSBError( "%s: missing file operand\n", argv[0]);
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
			lp.setPortState( p, how );
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
	return 0;
}



int main(int argc, char* argv[])
{
	LinuxPowerUSB p;
	p.setDebug(false);
	p.Setup();
	try {
		return handleRequest(p, argc, argv);
	} catch( LinuxPowerUSBError &ex ) {
		p.error( "%s\n", ex.what() );
	}
	return 2;
}
